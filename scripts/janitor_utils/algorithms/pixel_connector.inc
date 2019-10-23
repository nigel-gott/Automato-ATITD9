dofile("janitor_utils/util/class.inc")
dofile("janitor_utils/algorithms/array_2d.inc")

function test_pixel_connector()
  local pixels = Array2D {
    { 1, 1, 0, 1, 1 },
    { 0, 0, 0, 1, 1 },
    { 1, 1, 0, 1, 1 },
    { 0, 0, 1, 1, 1 },
    { 1, 0, 1, 1, 1 },
  }
  assert(pixels:get(1, 1) == 1, tostring(pixels:get(1, 1)))
  assert(pixels:get(3, 1) == 0, tostring(pixels:get(1, 1)))
  local connector = PixelConnector(pixels, function(new, old)
    return new.value == 1 and (not old or old.value == 1)
  end)

  connector:connect()

  local expected_labelling = {
    { 1, 1, 0, 2, 2 },
    { 0, 0, 0, 2, 2 },
    { 3, 3, 0, 2, 2 },
    { 0, 0, 2, 2, 2 },
    { 4, 0, 2, 2, 2 },
  }
  --print_labelling(connector.labelled_pixels)
  assert_labelling_matches(connector.labelled_pixels, expected_labelling)
  assert(connector.labels[1].pixels.size == 2)
  assert(connector.labels[2].pixels.size == 12)
  assert(connector.labels[3].pixels.size == 2)
  assert(connector.labels[4].pixels.size == 1)

  assert(connector:max_label().pixels.size == 12)
end

function print_labelling(labels)
  local str = ""
  for y, row in ipairs(labels) do
    for x, label in ipairs(row) do
      str = str .. label.id .. ","
    end
    str = str .. "\n"
  end
  print(str)
end

function assert_labelling_matches(actual_labels, expected_labelling)
  for y, row in ipairs(expected_labelling) do
    for x, label in ipairs(row) do
      assert(actual_labels[y])
      assert(actual_labels[y][x].id == label, "at " .. x .. "," .. y .. " - " .. actual_labels[y][x].id .. " != " .. label)
    end
  end
end

PixelConnector = class { class_name = "PixelConnector" }
function PixelConnector:init(pixels, connected_func, getter_func)
  self.pixels = pixels
  self.connected_func = connected_func
  self.getter_func = getter_func
  self.labelled_pixels = {}
  self.labels = { ["nil"] = { id = 0 }, { id = 1, pixels = Array2D() } }
  self.nil_label = self.labels["nil"]
  self.current_label = self.labels[1]
end
function PixelConnector:not_labelled(pixel)
  return not (self.labelled_pixels[pixel.xy.y] and self.labelled_pixels[pixel.xy.y][pixel.xy.x]) or self.labelled_pixels[pixel.xy.y][pixel.xy.x] == self.nil_label
end

function PixelConnector:connect()
  for xy, value in self.pixels:iterate(true) do
    --print("Connecting " .. tostring(xy))
    self:label_connecting_pixels(xy)
  end
end

function PixelConnector:label_connecting_pixels(xy)
  self:search_for_and_collect_connected_pixels { { xy = xy, previous_pixel = false } }
  if self.current_label.pixels.size > 1 then
    self.current_label = { id = self.current_label.id + 1, pixels = Array2D() }
    table.insert(self.labels, self.current_label)
  end
end

function PixelConnector:search_for_and_collect_connected_pixels(pixels_to_check)
  while #pixels_to_check > 0 do
    --print(#pixels_to_check .. " remaining to check")
    local next_pixel = table.remove(pixels_to_check)
    if self:not_labelled(next_pixel) and self.pixels:inside(next_pixel.xy) then
      next_pixel.value = self.pixels:get(next_pixel.xy)
      if self.getter_func then
        next_pixel.value = self.getter_func(next_pixel)
      end
      if next_pixel.value and self:connected(next_pixel, next_pixel.previous_pixel) then
        --print("Adding " .. tostring(next_pixel.xy) .. "=" .. next_pixel.value .. " to label " .. self.current_label.id)
        self:add_pixel(next_pixel, self.current_label)
        self:queue_up_surrounding_pixels(next_pixel, pixels_to_check)
      else
        --print("Skipping " .. tostring(next_pixel.xy) .. "=" .. tostring(next_pixel.value) .. " for label " .. self.current_label.id)
        self:add_pixel(next_pixel, self.nil_label)
      end
    end
  end
end

function PixelConnector:connected(new_pixel, previous_pixel)
  return self.connected_func(new_pixel, previous_pixel)
end

function PixelConnector:add_pixel(pixel, label)
  if label.pixels then
    label.pixels:add(pixel.xy, pixel.value)
  end
  self.labelled_pixels[pixel.xy.y] = self.labelled_pixels[pixel.xy.y] or {}
  self.labelled_pixels[pixel.xy.y][pixel.xy.x] = label
end

function PixelConnector:queue_up_surrounding_pixels(p, pixels_to_check)
  local locations = {
    { x = 1, y = 0, },
    { x = -1, y = 0, },
    { x = 0, y = 1, },
    { x = 0, y = -1, }
  }
  for _, location in ipairs(locations) do
    table.insert(pixels_to_check, { xy = p.xy + Point2D(location.x, location.y, p.xy.system), previous_pixel = p })
  end
end
function PixelConnector:max_label()
  local max_size = 0
  local max_index = false
  for i, label in ipairs(self.labels) do
    local size = label.pixels.size
    if not max_index or size > max_size then
      max_size = size
      max_index = i
    end
  end
  if not max_index or max_size == 0 then
    return false
  else
    return self.labels[max_index]
  end
end

test_pixel_connector()
