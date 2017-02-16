require_relative "../../lib/kata.rb"

describe "Kata" do
  it "should convert 3 to Fizz"  do
    expect(FizzBuzz.convert(3)).to eq('Fizz')
  end

  it "should convert 5 to Buzz"  do
    expect(FizzBuzz.convert(5)).to eq('Buzz')
  end
  it "should convert 15 to FizzBuzz"  do
    expect(FizzBuzz.convert(15)).to eq('FizzBuzz')
  end
end
