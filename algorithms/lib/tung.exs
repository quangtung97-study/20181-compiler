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

alias Compiler.LL, as: LL

k = 1
firsts = LL.first_map(vt, vn, p, k)
follows = LL.follow_map(vn, p, :S, firsts, k)

IO.puts(LL.to_string(firsts))
IO.puts("-------------------------------")
IO.puts(LL.to_string(follows))
