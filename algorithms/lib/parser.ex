defmodule Parser do
  def tree(parent, childs) do
    {parent, childs}
  end

  defp g({[:a | tail], trees}) do
    a = Parser.tree(:a, [])
    tree = Parser.tree(:G, [a])
    {tail, [tree | trees]}
  end

  defp g({[:"(" | tail], trees}) do
    {remains, [e_subtree]} = {tail, []} |> e()

    case remains do
      [:")" | xs] ->
        lparent = Parser.tree(:"(", [])
        rparent = Parser.tree(:")", [])
        tree = Parser.tree(:G, [lparent, e_subtree, rparent])
        {xs, [tree | trees]}

      _ ->
        raise("expected a right parenthesis")
    end
  end

  defp g({_, _trees}), do: raise("expected an operand")

  defp f1({[:* | tail], trees}) do
    {remains, subtrees} =
      {tail, []}
      |> g()
      |> f1()

    times = Parser.tree(:*, [])
    tree = Parser.tree(:F1, [times | Enum.reverse(subtrees)])
    {remains, [tree | trees]}
  end

  defp f1({tokens, trees}) do
    tree = Parser.tree(:F1, [])
    {tokens, [tree | trees]}
  end

  defp f({tokens, trees}) do
    {remains, subtrees} =
      {tokens, []}
      |> g()
      |> f1()

    tree = Parser.tree(:F, Enum.reverse(subtrees))
    {remains, [tree | trees]}
  end

  defp e1({[:+ | tail], trees}) do
    {remains, subtrees} =
      {tail, []}
      |> f()
      |> e1()

    plus = Parser.tree(:+, [])
    tree = Parser.tree(:E1, [plus | Enum.reverse(subtrees)])
    {remains, [tree | trees]}
  end

  defp e1({tokens, trees}) do
    tree = Parser.tree(:E1, [])
    {tokens, [tree | trees]}
  end

  defp e({tokens, trees}) do
    {remains, subtrees} =
      {tokens, []}
      |> f()
      |> e1()

    tree = Parser.tree(:E, Enum.reverse(subtrees))
    {remains, [tree | trees]}
  end

  def parse(tokens) do
    {remains, [tree]} = {tokens, []} |> e()

    case remains do
      [] -> tree
      list -> raise("remains: #{inspect(list)}")
    end
  end
end
