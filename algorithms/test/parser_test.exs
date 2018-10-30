defmodule ParserTest do
  use ExUnit.Case
  doctest Parser

  test "build parse tree" do
    g = Parser.tree(:a, [])
    tree = Parser.tree(:E, [g])
    assert tree == {:E, [{:a, []}]}
  end

  test "parsing simple expression" do
    tokens = [:a]
    parse_tree = Parser.parse(tokens)

    #       E
    #    F     E1
    #   G F1   e
    #   a  e
    #   e
    a = Parser.tree(:a, [])
    g = Parser.tree(:G, [a])
    f1 = Parser.tree(:F1, [])
    f = Parser.tree(:F, [g, f1])
    e1 = Parser.tree(:E1, [])
    e = Parser.tree(:E, [f, e1])

    assert parse_tree == e
  end

  test "parsing plus expression" do
    tokens = [:a, :+, :a]
    parse_tree = Parser.parse(tokens)

    #             E
    #         F         E1
    #       G  F1    +   F        E1 
    #       a  e     e   G  F1    e
    #       e            a  e
    #                    e
    a = Parser.tree(:a, [])
    g = Parser.tree(:G, [a])
    f1 = Parser.tree(:F1, [])
    f = Parser.tree(:F, [g, f1])
    e1_e = Parser.tree(:E1, [])
    plus = Parser.tree(:+, [])
    e1 = Parser.tree(:E1, [plus, f, e1_e])
    e = Parser.tree(:E, [f, e1])

    assert parse_tree == e
  end
end
