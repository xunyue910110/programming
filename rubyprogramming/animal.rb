class Animal
#  attr_accessor :name
  attr_reader :name
  attr_writer :name
  def initialize(name)
    @name = name
  end

end

h = Animal.new "hannah"
puts h.name
h.name = "jun"
h.name="zhoa"
puts h.name
