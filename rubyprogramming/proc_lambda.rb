def proc_test
  puts "hello"
  my_proc = Proc.new { return 1}
  my_proc.call
  puts "ruby"
end
  def lambda_test
  puts "hello"
  my_lambda = -> { return 1}
  my_lambda.call
  puts "ruby"
  end

proc_test
lambda_test
