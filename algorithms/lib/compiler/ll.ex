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

  defp follow_to_tuples({_a, []}, _follows, _firsts, _k), do: []

  defp follow_to_tuples({a, list}, follows, firsts, k) do
    nonterminal? = &Map.has_key?(follows, &1)

    {hd(list), tl(list)}
    |> Stream.iterate(fn {_, tail} -> {hd(tail), tl(tail)} end)
    |> Stream.take(length(list))
    |> Stream.filter(fn {head, _} -> nonterminal?.(head) end)
    |> Stream.map(fn {b, list} ->
      {b, follow_concat(list, a, follows, firsts, k)}
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
