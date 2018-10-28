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

  defp first_concat(list, map, k) do
    single = MapSet.new([[]])

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
end
