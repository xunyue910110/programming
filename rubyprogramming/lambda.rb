my_lambda = lambda{|x| x ** 2}

my_lambda2 = ->  x {  x ** 2 }

p my_lambda2.call(10)
p my_lambda2[10]
