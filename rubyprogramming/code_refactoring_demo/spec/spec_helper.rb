require 'factory_girl'
require 'faker'

require_relative '../lib/customer'
require_relative '../lib/movie'
require_relative '../lib/rental'

RSpec.configure do |config|
  config.expect_with :rspec do |expectations|
    expectations.include_chain_clauses_in_custom_matcher_descriptions = true
  end

  config.mock_with :rspec do |mocks|
    mocks.verify_partial_doubles = true
  end

  # factory girl settings
  config.include FactoryGirl::Syntax::Methods
  config.before do
    FactoryGirl.reload
  end
end
