END{
  puts "end"
}
BEGIN{
puts "begin"
}
unless  false
  puts "This is true"
end

puts "true" if true
a = 1
puts "hello" if a < 2

a = 2
unless a > 2
  puts "hello"
end

a = 2
case
when 1 == 1
  puts "bb"
when a == 2
  puts "aa"
end

puts "xx" if a == 2

b = (a == true) ? 1 : 2

[1,2,3,4,5].each { |i|
  puts i
}
puts "next"
[1,2,3,4,5].each_with_index{ |i,index|
  puts index
  puts i
}

