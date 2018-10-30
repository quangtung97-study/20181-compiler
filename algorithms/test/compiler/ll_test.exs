defmodule Compiler.LLTest do
  use ExUnit.Case
  doctest Compiler.LL
  alias Compiler.LL, as: LL

  test "using MapSet" do
    assert MapSet.new() == MapSet.new()
  end

  test "append(a, b, k)" do
    assert LL.append([1, 2], [2, 3], 3) == [1, 2, 2]
    assert LL.append([], [], 1) == []
  end

  test "append_set(a, b, k)" do
    a = [[1, 2], [4, 5]] |> MapSet.new()
    b = [[7, 8], [9, 4]] |> MapSet.new()

    c =
      [
        [1, 2, 7],
        [1, 2, 9],
        [4, 5, 7],
        [4, 5, 9]
      ]
      |> MapSet.new()

    assert LL.append_set(a, b, 3) == c

    a = [[]] |> MapSet.new()
    assert LL.append_set(a, b, 3) == b
  end

  test "calculate first map" do
    vt = [:+, :*, :"(", :")", :a]
    vn = [:S, :A, :B, :C, :D]

    p = [
      S: [:B, :A],
      A: [:+, :B, :A],
      A: [],
      B: [:D, :C],
      C: [:*, :D, :C],
      C: [],
      D: [:"(", :S, :")"],
      D: [:a]
    ]

    map = LL.first_map(vt, vn, p, 1)
    s = [[:"("], [:a]] |> MapSet.new()
    c = [[:*], []] |> MapSet.new()

    assert Map.get(map, :S) == s
    assert Map.get(map, :B) == s
    assert Map.get(map, :D) == s
    assert Map.get(map, :C) == c
  end

  test "calculate follow map" do
    vt = [:+, :*, :"(", :")", :a]
    vn = [:S, :A, :B, :C, :D]

    p = [
      S: [:B, :A],
      A: [:+, :B, :A],
      A: [],
      B: [:D, :C],
      C: [:*, :D, :C],
      C: [],
      D: [:"(", :S, :")"],
      D: [:a]
    ]

    firsts = LL.first_map(vt, vn, p, 1)
    follows = LL.follow_map(vn, p, :S, firsts, 1)
    s = [[], [:")"]] |> MapSet.new()
    a = [[], [:")"]] |> MapSet.new()
    b = [[:+], [], [:")"]] |> MapSet.new()
    c = [[:+], [], [:")"]] |> MapSet.new()
    d = [[:+], [:*], [], [:")"]] |> MapSet.new()

    assert Map.get(follows, :S) == s
    assert Map.get(follows, :A) == a
    assert Map.get(follows, :B) == b
    assert Map.get(follows, :C) == c
    assert Map.get(follows, :D) == d
  end

  test "disjoint_asymmetric_pairs" do
    compared = [
      {1, 2},
      {1, 3},
      {1, 5},
      {2, 3},
      {2, 5},
      {3, 5}
    ]

    result = LL.disjoint_asymmetric_pairs([1, 2, 3, 5]) |> Enum.to_list()
    assert result == compared
  end

  test "LL(k) checking => true" do
    vt = [:+, :*, :"(", :")", :a]
    vn = [:S, :A, :B, :C, :D]

    p = [
      S: [:B, :A],
      A: [:+, :B, :A],
      A: [],
      B: [:D, :C],
      C: [:*, :D, :C],
      C: [],
      D: [:"(", :S, :")"],
      D: [:a]
    ]

    firsts = LL.first_map(vt, vn, p, 1)
    follows = LL.follow_map(vn, p, :S, firsts, 1)

    assert LL.ll?(vn, p, firsts, follows, 1) == true
  end

  test "LL(k) checking => false" do
    vt = [:a]
    vn = [:S, :A, :B]

    p = [
      S: [:A],
      S: [:B],
      A: [:a],
      B: [:a]
    ]

    firsts = LL.first_map(vt, vn, p, 1)
    follows = LL.follow_map(vn, p, :S, firsts, 1)

    a = [[:a]] |> MapSet.new()
    compared = {:error, [{:S, {[:A], a}, {[:B], a}}]}
    assert LL.ll?(vn, p, firsts, follows, 1) == compared
  end
end
