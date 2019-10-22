dofile("janitor_utils/util/class.inc")
dofile("janitor_utils/algorithms/array_2d.inc")

function test_shape()
  local shape = Shape(Array2D{
    { 0, 1, 0, 0 },
    { 0, 1, 1, 1 },
    { 1, 1, 1, 1 },
    { 0, 1, 1, 1 },
    nil_value = 0,
  } )
  assert(shape.pixels.width == 4, shape.pixels.width)
  assert(shape.pixels.height == 4)
  assert(shape.anchor_point.x == 3, shape.anchor_point.x)
  assert(shape.anchor_point.y == 3)
  local expected = {
    { 0, 1, 0, 0 },
    { 0, 1, 1, 1 },
    { 1, 1, 1, 1 },
    { 0, 1, 1, 1 },
  }
  for y, row in ipairs(expected) do
    for x, value in ipairs(row) do
      if value == 0 then
        assert(not shape.pixels:get(x,y))
      else
        assert(shape.pixels:get(x,y)== value)
      end
    end
  end
end

Shape = class { class_name = "Shape" }
function Shape:init(pixels)
  self.pixels = pixels
  self:find_anchor_point()
end

function Shape:find_anchor_point()
  local inner_pixels = self.pixels
  local next_inner_pixels = Array2D()
  local last_pixel
  local count = 0
  while count < self.pixels.size do
    for xy, value in inner_pixels:iterate(false) do
      local up = inner_pixels:get(xy.x, xy.y - 1)
      local down = inner_pixels:get(xy.x, xy.y + 1)
      local left = inner_pixels:get(xy.x - 1, xy.y)
      local right = inner_pixels:get(xy.x + 1, xy.y)
      if up and down and left and right then
        next_inner_pixels:add(xy, value)
      else
        count = count + 1
      end
      last_pixel = xy
    end
    inner_pixels = next_inner_pixels
    next_inner_pixels = Array2D()
  end
  self.anchor_point = last_pixel
end

test_shape()
