defmodule AST do
  def tree(parent, children) do
    {parent, children}
  end

  defp convert_e1(ast, {:E1, []}), do: ast

  defp convert_e1(ast, {:E1, [{:+, []}, f, e1]}) do
    left = AST.tree(:+, [ast, convert(f)])
    convert_e1(left, e1)
  end

  defp convert_f1(ast, {:F1, []}), do: ast

  defp convert_f1(ast, {:F1, [{:*, []}, g, f1]}) do
    left = AST.tree(:*, [ast, convert(g)])
    convert_f1(left, f1)
  end

  def convert({:E, [f, e1]}) do
    convert_e1(convert(f), e1)
  end

  def convert({:F, [g, f1]}) do
    convert_f1(convert(g), f1)
  end

  def convert({:G, [a]}), do: convert(a)

  def convert({:G, [{:"(", []}, a, {:")", []}]}) do
    convert(a)
  end

  def convert({:a, []}), do: :a
end
