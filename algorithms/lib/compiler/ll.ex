defmodule Compiler.LL do
  def append(a, b, k) when is_list(a) and is_list(b) do
    Enum.take(a ++ b, k)
  end

  def append_set(a, b, k) do
    a
    |> MapSet.to_list()
    |> Stream.flat_map(fn e ->
      Stream.map(b, &append(e, &1, k))
    end)
    |> MapSet.new()
  end

  # For FIRST computation

  defp first_concat(list, map, k) do
    # %{[]}
    single = [[]] |> MapSet.new()

    list
    |> Stream.map(&Map.get(map, &1))
    |> Enum.reduce(single, &append_set(&2, &1, k))
  end

  defp first_next({_, map}, p, k) do
    union_to_map = fn {key, set}, m ->
      old_set = Map.get(m, key)
      new_set = MapSet.union(set, old_set)
      Map.put(m, key, new_set)
    end

    new_map =
      p
      |> Stream.map(fn {a, list} ->
        {a, first_concat(list, map, k)}
      end)
      |> Enum.reduce(map, union_to_map)

    {new_map == map, new_map}
  end

  def first_map(vt, vn, p, k) do
    empty = MapSet.new()
    single = &MapSet.new([[&1]])

    vt_tuples = Stream.map(vt, &{&1, single.(&1)})
    vn_tuples = Stream.map(vn, &{&1, empty})

    {true, new_map} =
      {false, Map.new(Stream.concat(vt_tuples, vn_tuples))}
      |> Stream.iterate(&first_next(&1, p, k))
      |> Enum.find(fn {terminated, _} -> terminated end)

    new_map
  end

  # For FOLLOW computation

  defp follow_concat(list, a, follows, firsts, k) do
    single = [[]] |> MapSet.new()

    list
    |> Stream.map(&Map.get(firsts, &1))
    |> Enum.reduce(single, &append_set(&2, &1, k))
    |> append_set(Map.get(follows, a), k)
  end

  defp follow_to_tuples({a, list}, follows, firsts, k) do
    nonterminal? = &Map.has_key?(follows, &1)

    unfold_func = fn
      [] -> nil
      list -> {{hd(list), tl(list)}, tl(list)}
    end

    list
    |> Stream.unfold(unfold_func)
    |> Stream.filter(fn {head, _} -> nonterminal?.(head) end)
    |> Stream.map(fn {head, list} ->
      {head, follow_concat(list, a, follows, firsts, k)}
    end)
  end

  defp follow_next({_, follows}, p, firsts, k) do
    union_to_map = fn {key, set}, m ->
      old_set = Map.get(m, key)
      new_set = MapSet.union(set, old_set)
      Map.put(m, key, new_set)
    end

    new_follows =
      p
      |> Stream.flat_map(&follow_to_tuples(&1, follows, firsts, k))
      |> Enum.reduce(follows, union_to_map)

    {new_follows == follows, new_follows}
  end

  def follow_map(vn, p, s, firsts, k) do
    empty = MapSet.new()
    single_epsilon = [[]] |> MapSet.new()

    vn_tuples = Stream.map(vn, &{&1, empty})

    follows =
      vn_tuples
      |> Map.new()
      |> Map.put(s, single_epsilon)

    {true, new_map} =
      {false, follows}
      |> Stream.iterate(&follow_next(&1, p, firsts, k))
      |> Enum.find(fn {terminated, _} -> terminated end)

    new_map
  end

  # For LL(k) checking

  defp compute_production_map(vn, p) do
    pmap =
      vn
      |> Stream.map(&{&1, []})
      |> Map.new()

    pmap_reduce = fn {a, list}, map ->
      tail = Map.get(map, a)
      Map.put(map, a, [list | tail])
    end

    p
    |> Enum.reduce(pmap, pmap_reduce)
    |> Map.to_list()
    |> Stream.map(fn {key, list} -> {key, Enum.reverse(list)} end)
    |> Map.new()
  end

  def disjoint_asymmetric_pairs(input_list) do
    input_list
    |> Stream.unfold(fn
      [] -> nil
      list -> {{hd(list), tl(list)}, tl(list)}
    end)
    |> Stream.flat_map(fn {e1, list} ->
      Stream.map(list, fn e2 -> {e1, e2} end)
    end)
  end

  defp check_ll_production({a, lists}, firsts, follows, k) do
    follow_a = Map.get(follows, a)

    reducer = fn {{list1, set1}, {list2, set2}}, errors ->
      case MapSet.disjoint?(set1, set2) do
        true -> errors
        false -> [{a, {list1, set1}, {list2, set2}} | errors]
      end
    end

    lists
    |> Stream.map(&{&1, first_concat(&1, firsts, k)})
    |> Stream.map(fn {list, set} ->
      {list, append_set(set, follow_a, k)}
    end)
    |> Enum.to_list()
    |> disjoint_asymmetric_pairs()
    |> Enum.reduce([], reducer)
  end

  def ll?(vn, p, firsts, follows, k) do
    compute_production_map(vn, p)
    |> Map.to_list()
    |> Enum.flat_map(&check_ll_production(&1, firsts, follows, k))
    |> (fn
          [] -> true
          list -> {:error, list}
        end).()
  end

  # For displaying

  def to_string(map) do
    list_to_string = fn list ->
      list
      |> Stream.map(&Kernel.to_string/1)
      |> Enum.join("")
      |> (fn
            "" -> "𝛆"
            s -> s
          end).()
    end

    set_to_string = fn set ->
      set
      |> MapSet.to_list()
      |> Stream.map(list_to_string)
      |> Enum.join(", ")
      |> (&"{#{&1}}").()
    end

    tuple_to_string = fn {a, set} ->
      Kernel.to_string(a) <> ": " <> set_to_string.(set)
    end

    map
    |> Map.to_list()
    |> Stream.map(tuple_to_string)
    |> Enum.join("\n")
  end
end
