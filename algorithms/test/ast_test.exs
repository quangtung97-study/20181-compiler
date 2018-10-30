defmodule ASTTest do
  use ExUnit.Case
  doctest AST

  test "convert simple parse tree" do
    tokens = [:a, :+, :a, :+, :a]
    ast = AST.tree(:+, [:a, :a])
    ast = AST.tree(:+, [ast, :a])

    parse_tree = Parser.parse(tokens)
    assert AST.convert(parse_tree) == ast
  end

  test "convert parse tree" do
    tokens = [:a, :+, :a, :*, :a, :+, :a]
    ast = AST.tree(:*, [:a, :a])
    ast = AST.tree(:+, [:a, ast])
    ast = AST.tree(:+, [ast, :a])

    parse_tree = Parser.parse(tokens)
    assert AST.convert(parse_tree) == ast
  end

  test "convert (have parentheses) parse tree" do
    tokens = [:a, :*, :a, :*, :"(", :a, :+, :a, :")"]
    ast1 = AST.tree(:*, [:a, :a])
    ast2 = AST.tree(:+, [:a, :a])
    ast = AST.tree(:*, [ast1, ast2])

    parse_tree = Parser.parse(tokens)
    assert AST.convert(parse_tree) == ast
  end
end
