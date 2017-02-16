class Movie
  REGULAR     = 0     # 普通片
  NEW_RELEASE = 1     # 新片
  CHILDRENS   = 2     # 兒童片

  attr_reader :title
  attr_accessor :price_code

  def initialize(title, price_code)
    @title, @price_code = title, price_code
  end

  def charge(days_rented)
        this_amount = 0
      # determine amounts for each line
      case price_code
      when Movie::REGULAR
        this_amount += 2
        this_amount += (days_rented - 2) * 1.5 if days_rented > 2
      when Movie::NEW_RELEASE
        this_amount += days_rented * 3
      when Movie::CHILDRENS
        this_amount += 1.5
        this_amount += (days_rented - 3) * 1.5 if days_rented > 3
      end
      this_amount
  end

  def  frequent_renter_points(days_rented)
      # 累加常客積點
      frequent_renter_points = 1
      # 如果是新片而且租超過 1 天，另外加 1 點
      if price_code == Movie::NEW_RELEASE && days_rented > 1
        frequent_renter_points += 1
      end
      frequent_renter_points
  end

end

