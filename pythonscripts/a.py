def calc_method(a, b = 1):
    print a * b

def args_method(*args):
    print args

def kwargs_method(**kwargs):
    print kwargs

calc_method(1,2)
#args_method([1,2,3,4])
calc_method(4,10)
calc_method(b=10, a = 2)

args_method(1,2,3)
kwargs_method(name="eddie", age=20)

