dofile("veg_janitor/util.inc")
PixelConnector = {}
function PixelConnector:new(width, height, get_function)
  local o = {
    width = width,
    height = height,
    get_function = get_function,
    labels = {},
    current_label = { id = 1, pixels = {} },
    not_labelled = function(pixel)
      return not (self.labels[pixel.y] and self.labels[pixel.y][pixel.x])
    end
  }
  return newObject(self, o)
end

function PixelConnector:run()
  for y = 1, self.height do
    for x = 1, self.width do
      self:label_connecting_pixels(x, y)
    end
  end
end

function self:label_connecting_pixels(x, y)
  self:search_for_and_collect_connected_pixels({ x = x, y = y, previous_pixel = false })
  if #self.current_label.pixels > 1 then
    self.current_label = {id=self.current_label.id + 1, pixels = {}}
  end
end

function PixelConnector:search_for_and_collect_connected_pixels(pixels_to_check)
  while #pixels_to_check > 0 do
    local next_pixel = table.remove(pixels_to_check)
    if self:not_labelled(next_pixel) then
      next_pixel.value = self.get_function(next_pixel)
      if next_pixel.value and self:connected(next_pixel, next_pixel.previous_pixel) then
        self:add_pixel(next_pixel)
        self:queue_up_surrounding_pixels(next_pixel, pixels_to_check)
      end
    end
  end
end

function self:connected(new_pixel, previous_pixel)
  return true
end

function self:add_pixel(pixel)
  table.insert(self.current_label.pixels, pixel)
  self.labels[pixel.y] = self.labels[pixel.y] or {}
  self.labels[pixel.y][pixel.x] = pixel
end

function self:queue_up_surrounding_pixels(p, pixels_to_check)
  append(pixels_to_check, {
    { p.x + 1, p.y, previous_pixel = p },
    { p.x - 1, p.y, previous_pixel = p },
    { p.x, p.y + 1, previous_pixel = p},
    { p.x, p.y - 1, previous_pixel = p},
  })
end

function append(table_to_append_to, table)
  for _, value in ipairs(table) do
    table.insert(table_to_append_to, value)
  end
end


ShapeComparer = {}
function ShapeComparer:new()
  local o = {}
  return newObject(self, o)
end

function ShapeComparer:compare(target_shape, candidate_shape)
  -- Move in a spiral out from the anchor point of target, starting by overlaping anchor's and then moving the candidate anchor around
  local num_matching_pixels = 0
  for y, row in ipairs(target_shape) do
    for x, pixel in ipairs(row) do
      if candidate_shape[y] and candidate_shape[y][x] then
        if pixel and pixel ~= 0 and pixel == candidate_shape[y][x] then
          num_matching_pixels = num_matching_pixels + 1
        end
      end
    end
  end
  return ComparisionResult:new { num_matching_pixels = num_matching_pixels, matches = num_matching_pixels >= (target_shape.size * 0.8) }
end

ComparisionResult = {}
function ComparisionResult:new(o)
  return newObject(self, o)
end

Shape = {}
function Shape:new(o)
  return newObject(self, o)
end

function test_shape_detector()
  local target_shape = Shape:new {
    [1] = { 0, 0, 1, 1, 0, 0 },
    [2] = { 0, 1, 1, 1, 1, 0 },
    [3] = { 1, 1, 1, 1, 1, 1 },
    size = 12,
    width = 6,
    height = 3,
    anchor_point = { x = 4, y = 2 },
  }
  local matching_shape = Shape:new {
    [1] = { 0, 0, 1, 1, 0, 0 },
    [2] = { 0, 1, 1, 1, 1, 0 },
    [3] = { 1, 1, 1, 1, 1, 1 },
    size = 12,
    width = 6,
    height = 3,
    anchor_point = { x = 4, y = 2 },
  }
  local comparer = ShapeComparer:new()
  local matching_result = comparer:compare(target_shape, matching_shape)
  assert(matching_result.matches)
  assert(matching_result.num_matching_pixels == target_shape.size, matching_result.num_matching_pixels .. " != " .. target_shape.size)
  --assert(matching_result.anchor_point.x == 4)
  --assert(matching_result.anchor_point.y == 2)
  --assert(matching_result.matching_pixel_percentage == 100)
  --assert(shape:compare(correct_comparison_pixels):locat())
  --assert(shape:compare(close_enough_comparison_pixels))
  --assert(not shape:compare(wrong_comparision_pixels))
  --local close_enough_comparison_pixels = Pixels:new {
  --  [1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  --  [2] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  --  [3] = { 0, 0, 1, 1, 0, 0, 0, 0, 0 },
  --  [4] = { 0, 1, 1, 1, 1, 0, 0, 0, 0 },
  --  [5] = { 1, 1, 1, 1, 0, 0, 0, 0, 0 },
  --}
  --local wrong_comparision_pixels = Pixels:new {
  --  [1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  --  [2] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  --  [3] = { 0, 0, 1, 1, 0, 0, 0, 0, 0 },
  --  [4] = { 0, 1, 1, 1, 1, 0, 0, 0, 0 },
  --  [5] = { 1, 1, 1, 1, 0, 0, 0, 0, 0 },
  --}
end

test_shape_detector()
