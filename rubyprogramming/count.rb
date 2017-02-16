source = "abcdefg"

new_string = []
 source.split('') . each {|c|
   new_string.unshift(c)
}
 p new_string.join

list =%w(1 3 5 1 7 nil 7)
p list.sort


str = "abcdefg"
p str.chop
p str
