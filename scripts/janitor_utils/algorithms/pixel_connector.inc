dofile("janitor_utils/util/class.inc")

function test_pixel_connector()
  local pixels = {
    { 1, 1, 0, 1, 1 },
    { 0, 0, 0, 1, 1 },
    { 1, 1, 0, 1, 1 },
    { 0, 0, 1, 1, 1 },
    { 1, 0, 1, 1, 1 },
  }
  local connector = PixelConnector(5, 5, function(pixel)
    return pixels[pixel.y][pixel.x] ~= 0
  end)

  connector:connect()

  local expected_labelling = {
    { 1, 1, 0, 2, 2 },
    { 0, 0, 0, 2, 2 },
    { 3, 3, 0, 2, 2 },
    { 0, 0, 2, 2, 2 },
    { 4, 0, 2, 2, 2 },
  }
  assert_labelling_matches(connector.labelled_pixels, expected_labelling)
  assert(#connector.labels[1].pixels == 2)
  assert(#connector.labels[2].pixels == 12)
  assert(#connector.labels[3].pixels == 2)
  assert(#connector.labels[4].pixels == 1)
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
      assert(actual_labels[y][x].id == label, "at " .. x .. "," .. y .. " - " .. actual_labels[y][x].id .. " != " .. label)
    end
  end
end

PixelConnector = class { class_name = "PixelConnector" }
function PixelConnector:init(width, height, get_function)
  self.width = width
  self.height = height
  self.get_function = get_function
  self.labelled_pixels = {}
  self.labels = { ["nil"] = { id = 0 }, { id = 1, pixels = {} } }
  self.nil_label = self.labels["nil"]
  self.current_label = self.labels[1]
end
function PixelConnector:not_labelled(pixel)
  return not (self.labelled_pixels[pixel.y] and self.labelled_pixels[pixel.y][pixel.x]) or self.labelled_pixels[pixel.y][pixel.x] == self.nil_label
end

function PixelConnector:connect()
  for y = 1, self.height do
    for x = 1, self.width do
      self:label_connecting_pixels(x, y)
    end
  end
end

function PixelConnector:label_connecting_pixels(x, y)
  self:search_for_and_collect_connected_pixels { { x = x, y = y, previous_pixel = false } }
  if #self.current_label.pixels > 1 then
    self.current_label = { id = self.current_label.id + 1, pixels = {} }
    table.insert(self.labels, self.current_label)
  end
end

function PixelConnector:search_for_and_collect_connected_pixels(pixels_to_check)
  while #pixels_to_check > 0 do
    local next_pixel = table.remove(pixels_to_check)
    if self:not_labelled(next_pixel) then
      next_pixel.value = self.get_function(next_pixel)
      if next_pixel.value and self:connected(next_pixel, next_pixel.previous_pixel) then
        self:add_pixel(next_pixel, self.current_label)
        self:queue_up_surrounding_pixels(next_pixel, pixels_to_check)
      else
        self:add_pixel(next_pixel, self.nil_label)
      end
    end
  end
end

function PixelConnector:connected(new_pixel, previous_pixel)
  return true
end

function PixelConnector:add_pixel(pixel, label)
  if label.pixels then
    table.insert(label.pixels, pixel)
  end
  self.labelled_pixels[pixel.y] = self.labelled_pixels[pixel.y] or {}
  self.labelled_pixels[pixel.y][pixel.x] = label
end

function PixelConnector:queue_up_surrounding_pixels(p, pixels_to_check)
  local locations = {
    { x = p.x + 1, y = p.y, },
    { x = p.x - 1, y = p.y, },
    { x = p.x, y = p.y + 1, },
    { x = p.x, y = p.y - 1, }
  }
  for _, location in ipairs(locations) do
    self:queue_up_pixel_if_valid_location({ x = location.x, y = location.y, previous_pixel = p }, pixels_to_check)
  end
end

function PixelConnector:queue_up_pixel_if_valid_location(pixel, pixels_to_check)
  if pixel.x <= self.width and pixel.x >= 1 and pixel.y <= self.height and pixel.y >= 1 then
    table.insert(pixels_to_check, pixel)
  end
end

test_pixel_connector()
