def open_my_file(file_name)
  File.open file_name do |f|
    puts f.read
  end
end

begin
  #open_my_file("block_demo.r")
  open_my_file("exception.rb")
rescue =>e
  puts e
else
  puts "it's working good!"
ensure
  puts "this must be executed, no matter what"
end
