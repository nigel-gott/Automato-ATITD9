dofile("janitor_utils/util/class.inc")
dofile("janitor_utils/util/box.inc")
dofile("janitor_utils/algorithms/array_2d.inc")
dofile("janitor_utils/algorithms/pixel_connector.inc")
dofile("janitor_utils/algorithms/shape.inc")

function test_screen_differ()
  local before = {
    {1,1,1,1,1},
    {1,1,1,1,1},
    {1,1,1,1,1},
    {1,1,1,1,1},
    {1,1,1,1,1},
  }
  local after = {
    {1,1,1,1,1},
    {1,1,0,1,1},
    {1,0,0,0,1},
    {1,1,0,1,1},
    {1,1,1,1,1},
  }
  local current = before
  local differ = ScreenDiffer(
    Box(Vector(1,1,0),5,5),
    CoordinateSystem{name="screen"},
    function() end, function(x,y)
    return current[y][x]
  end)
  differ:snapshot_and_store()
  current = after
  differ:snapshot()
  local pixel_connector = PixelConnector(
    differ.before_pixels,
    function(a, b)
      return differ:connected(a, b)
    end, function(pixel)
      return differ:changed(pixel)
    end)
  pixel_connector:connect()
  assert(pixel_connector:max_label())
  assert(pixel_connector:max_label().pixels.size == 5)
  local  shape = Shape(pixel_connector:max_label().pixels)
  assert(shape.anchor_point == Point2D(3,3,differ.system))

end
ScreenDiffer = class { class_name = "ScreenDiffer" }
function ScreenDiffer:init(box, system, read_func, pixel_get_func)
  self.box = box
  self.system = system
  self.read_func = read_func
  self.pixel_get_func = pixel_get_func
end
function ScreenDiffer:snapshot()
  self.read_func()
end
function ScreenDiffer:snapshot_and_store()
  self.before_pixels = Array2D()
  self.read_func()
  for y=1,self.box.height do
    self.before_pixels[y] = {}
    for x=1,self.box.width do
      self.before_pixels:add(Point2D(x,y,self.system), self.pixel_get_func(self.box.x + x-1, self.box.y + y-1))
    end
  end
end

function ScreenDiffer:changed(pixel)
  if pixel then
    local new = self.pixel_get_func(pixel.xy.x + self.box.x-1, pixel.xy.y + self.box.y-1)
    return pixel.value ~= new and 1 or 0
  else
    return false
  end
end
function ScreenDiffer:connected(a,b)
  return a.value == 1 and (not b or b.value == 1)
end

test_screen_differ()
