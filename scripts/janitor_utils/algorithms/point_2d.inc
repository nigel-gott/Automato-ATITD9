dofile("janitor_utils/util/class.inc")

function test_point_2d()
  local onion_system = CoordinateSystem { name = "from_onion_centre" }
  local screen = CoordinateSystem { name = "screen" }
  onion_system:connect(Point2D(10, 10, screen))
  local first_onion = Point2D(0, 0, onion_system) -- 0,-5 in from_centre
  local second_onion = Point2D(10, 0, onion_system) -- 0,-5 in from_centre
  local first_onion_screen_location = screen:translate(first_onion)
  assert(first_onion_screen_location.x == 10)
  assert(first_onion_screen_location.y == 10)
  assert(first_onion_screen_location.system.name == "screen")
  local second_onion_screen_location = screen:translate(second_onion)
  assert(second_onion_screen_location.x == 20)
  assert(second_onion_screen_location.y == 10)
  assert(second_onion_screen_location.system.name == "screen")

end

Point2D = class { class_name = "Point2D" }
function Point2D:init(x, y, system)
  self.x = x
  self.y = y
  self.system = system
end

function Point2D:__add(point)
  local translated = self.system:translate(point)
  return Point2D(self.x + translated.x, self.y + translated.y, self.system)
end

function Point2D:__sub(point)
  local translated = self.system:translate(point)
  return Point2D(self.x - translated.x, self.y - translated.y, self.system)
end

function Point2D:__div(divisor)
  return Point2D(self.x / divisor, self.y / divisor, self.system)
end

function Point2D:__mul(multiplicand)
  return Point2D(self.x * multiplicand, self.y * multiplicand, self.system)
end
function Point2D:__unm()
  return self * -1
end

function Point2D:length()
  return math.sqrt(self.x ^ 2 + self.y ^ 2)
end

function Point2D:normalize()
  return self / self:length()
end

function Point2D:__tostring()
  return "Point2D(" .. self.x .. ", " .. self.y .. ", " .. self.system.name .. ") "
end

CoordinateSystem = class { class_name = "CoordinateSystem" }
function CoordinateSystem:init(o)
  self.name = o.name
  self.connections = { }
end

-- Offset is Point(0,0,self) in offset.system
function CoordinateSystem:connect(offset)
  self.connections[offset.system.name] = offset
  local inverse = -offset
  inverse.system = self
  offset.system.connections[self.name] = inverse
end

function CoordinateSystem:translate(point)
  if point.system == self then
    return point
  else
    local offset = self.connections[point.system.name]
    assert(offset, "Don't know how to translate from " .. point.system.name .. " to " .. self.name)
    local translated = point - offset
    translated.system = self
    return translated
  end
end

test_point_2d()
