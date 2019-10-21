dofile("janitor_utils/util/class.inc")

function test_shape()
  local shape = Shape {
    pixels = {
      { x = 3, y = 2, value = 1 },
      { x = 4, y = 3, value = 1 },
      { x = 3, y = 3, value = 1 },
      { x = 3, y = 4, value = 1 },
      { x = 2, y = 4, value = 1 },
      { x = 4, y = 4, value = 1 },
      { x = 4, y = 5, value = 1 },
      { x = 3, y = 5, value = 1 },
      { x = 5, y = 3, value = 1 },
      { x = 5, y = 4, value = 1 },
      { x = 5, y = 5, value = 1 },
    }
  }
  assert(shape.width == 4, shape.width)
  assert(shape.height == 4)
  assert(shape.anchor_point.x == 3)
  assert(shape.anchor_point.y == 3)
  local expected = {
    { 0, 1, 0, 0 },
    { 0, 1, 1, 1 },
    { 1, 1, 1, 1 },
    { 0, 1, 1, 1 },
  }
  local str = ""
  for y = 1, shape.height do
    for x = 1, shape.width do
      if shape.pixels[y] and shape.pixels[y][x] then
        str = str .. "1,"
      else
        str = str .. "0,"
      end
    end
    str = str .. "\n"
  end
  print(str)
  for y, row in ipairs(expected) do
    for x, value in ipairs(row) do
      if value == 0 then
        assert(not shape.pixels[y][x])
      else
        assert(shape.pixels[y][x].value == value)
      end
    end
  end
end

Shape = class { class_name = "Shape" }
function Shape:init(label)
  self:build_pixels(label)
  self:find_anchor_point()
end

function Shape:build_pixels(label)
  local min_x, min_y, max_x, max_y
  for _, pixel in ipairs(label.pixels) do
    if not min_x or pixel.x < min_x then
      min_x = pixel.x
    end
    if not min_y or pixel.y < min_y then
      min_y = pixel.y
    end
    if not max_x or pixel.x > max_x then
      max_x = pixel.x
    end
    if not max_y or pixel.y > max_y then
      max_y = pixel.y
    end
  end
  self.width = max_x - min_x + 1
  self.height = max_y - min_y + 1
  self.size = #label.pixels
  self.pixels = {}
  self.pixels_list = {}
  for _, pixel in ipairs(label.pixels) do
    local y = (pixel.y + 1) - min_y
    local x = (pixel.x + 1) - min_x
    self.pixels[y] = self.pixels[y] or {}
    self.pixels[y][x] = { x = x, y = y, value = pixel.value }
    table.insert(self.pixels_list, self.pixels[y][x])
  end
end
function Shape:find_anchor_point()
  local inner_pixels = self.pixels
  local next_inner_pixels = {}
  local last_pixel
  local count = 0
  while count < self.size do
    -- Must iterate in an ordered fashion so the anchor point is not dependant on the pixels list ordering
    for y = 1, self.height do
      for x = 1, self.width do
        local pixel = inner_pixels[y] and inner_pixels[y][x]

        if pixel then
          local up = inner_pixels[pixel.y - 1] and inner_pixels[pixel.y - 1][pixel.x]
          local down = inner_pixels[pixel.y + 1] and inner_pixels[pixel.y + 1][pixel.x]
          local left = inner_pixels[pixel.y][pixel.x + 1]
          local right = inner_pixels[pixel.y][pixel.x - 1]
          if up and down and left and right then
            next_inner_pixels[y] = next_inner_pixels[y] or {}
            next_inner_pixels[y][x] = pixel
          else
            count = count + 1
          end
          last_pixel = pixel
        end
      end
    end
    inner_pixels = next_inner_pixels
    next_inner_pixels = {}
  end
  self.anchor_point = last_pixel
end

test_shape()
