dofile("janitor_utils/util/class.inc")
dofile("janitor_utils/algorithms/point_2d.inc")

function test_array_2d()
  local array = Array2D()
  local screen = CoordinateSystem { name = "screen" }
  array:add(Point2D(10, 10, screen), 1)
  array:add(Point2D(11, 11, screen), 1)
  array:add(Point2D(12, 12, screen), 1)

  local str = ""
  for xy, value in array:iterate(true) do
    if value then
      str = str .. (tostring(value))
    else
      str = str .. "0"
    end
    if xy.x == array.width then
      str = str .. "\n"
    end
  end
  assert(str == "100\n010\n001\n")
  local str = ""
  for xy, value in array:iterate(false) do
    str = str .. (tostring(value))
  end
  assert(str == "111")
  assert(array:get(10,10) == 1)
  assert(not array:get(11,10) )
end

function test_array_factory()
  local pixels = {
    { 1, 1, 0, 1, 1 },
    { 0, 0, 0, 1, 1 },
    { 1, 1, 0, 1, 1 },
    { 0, 0, 1, 1, 1 },
    { 1, 0, 1, 1, 1 },
  }
  local array = Array2D(pixels)
  for xy, value in array:iterate() do
    assert(pixels[xy.y][xy.x] == value)
  end
  assert(array.width == 5)
  assert(array.height == 5)
  assert(array.min_x == 1)
  assert(array.min_y == 1)
  assert(array.max_y == 5)
  assert(array.max_x == 5)
end

Array2D = class { class_name = "Array2D" }
function Array2D:init(o)
  self.min_x = false
  self.min_y = false
  self.max_x = false
  self.max_y = false
  self.array = {}
  self.size = 0
  if o and #o > 0 then
    self.system = o.system or CoordinateSystem { name = "default_system_" .. tostring(o) }
    for y, row in ipairs(o) do
      for x, value in ipairs(row) do
        self:add(Point2D(x, y, self.system), value)
      end
    end
  end
end

function Array2D:get(x,y)
  return self.array[y] and self.array[y][x]
end

function Array2D:add(point, value)
  if not self.system then
    self.system = point.system
  end
  if not self.min_x or point.x < self.min_x then
    self.min_x = point.x
  end
  if not self.min_y or point.y < self.min_y then
    self.min_y = point.y
  end
  if not self.max_x or point.x > self.max_x then
    self.max_x = point.x
  end
  if not self.max_y or point.y > self.max_y then
    self.max_y = point.y
  end
  self.width = self.max_x - self.min_x + 1
  self.height = self.max_y - self.min_y + 1
  self.size = self.size + 1
  self.array[point.y] = self.array[point.y] or {}
  self.array[point.y][point.x] = value
end

local function iterate_including_missing (array, xy)
  xy.x = xy.x + 1
  if xy.x > array.width then
    xy.x = 1
    xy.y = xy.y + 1
  end
  if xy.y > array.height then
    return nil
  end
  local offseted_y = xy.y + (array.min_y - 1)
  local offseted_x = xy.x + (array.min_x - 1)
  local v = array.array[offseted_y] and array.array[offseted_y][offseted_x]
  if array.array[offseted_y] and v ~= nil then
    return xy, v
  else
    return xy, false
  end
end

local function iterate_skipping_missing (array, xy)
  while true do
    local xy, value = iterate_including_missing(array, xy)
    if not xy then
      return nil
    elseif value then
      return xy, value
    end
  end
end
function Array2D:iterate(include_missing)
  local iter = include_missing and iterate_including_missing or iterate_skipping_missing
  return iter, self, { x = 0, y = 1 }
end

test_array_2d()
test_array_factory()
