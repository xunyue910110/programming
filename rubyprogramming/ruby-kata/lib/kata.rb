class FizzBuzz

  def self.convert(n)
    if ( n % 15 == 0) 
      'FizzBuzz'
    elsif ( n % 3 == 0 )
      'Fizz'
    elsif ( n % 5 == 0)
      'Buzz'
    end
  end
end
