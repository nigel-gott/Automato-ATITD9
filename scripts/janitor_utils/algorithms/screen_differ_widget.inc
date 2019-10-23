dofile("janitor_utils/util/class.inc")
dofile("janitor_utils/ui/colours.inc")
dofile("janitor_utils/algorithms/shape.inc")
dofile("janitor_utils/algorithms/screen_differ.inc")
dofile("janitor_utils/algorithms/pixel_connector.inc")

ScreenDifferWidget = class { class_name = "ScreenDifferWidget" }
function ScreenDifferWidget:init()
  self.state = "initial"
  local xyScreenSize = srGetWindowSize();
  local box_size = 600
  self.differ = ScreenDiffer(
    Box(Vector(xyScreenSize[0] / 2 - box_size, xyScreenSize[1] / 2 - box_size, 0),
       box_size*2, box_size*2),
    CoordinateSystem { name = "screen" },
    srReadScreen,
    srReadPixelFromBuffer
  )
end

function ScreenDifferWidget:render(render_box)
  if self.state == "initial" then
    return Button { text = "Snapshot Before", on_pressed = function()
      self.differ:snapshot_and_store()
      self.state = "snapshotted"
    end }
  elseif self.state == "snapshotted" then
    return Button { text = "Snapshot After", on_pressed = function()
      self.differ:snapshot()
      assert(self.differ.before_pixels.size > 10)
      self.pixel_connector = self.differ:connector()
      local start = lsGetTimer()
      print('Starting connect')
      self.pixel_connector:connect()
      print('connect took ' .. (lsGetTimer()-start)/1000 .. "s")
      if not self.pixel_connector:max_label() then
        self.state = "initial"
        return
      end
      assert(self.pixel_connector:max_label().pixels)
      assert(self.pixel_connector:max_label().pixels.size > 0)
      self.shape = Shape(self.pixel_connector:max_label().pixels)
      self.state = "diffed"
    end }
  elseif self.state == "diffed" then
    return ShapeWidget(self.shape)
  end
end

ShapeWidget = class { class_name = "ShapeWidget" }
function ShapeWidget:init(shape)
  self.shape = shape

end
function ShapeWidget:render(render_box)
  for xy, pixel in self.shape.pixels:iterate() do
    local colour
    if xy == self.shape.anchor_point then
      colour = RED
    else
      colour = WHITE
    end
    lsDisplaySystemSprite(1, render_box.x + xy.x - self.shape.pixels.min_x, render_box.y + xy.y - self.shape.pixels.min_y, render_box.z, 1, 1, colour);
  end
  return RenderResult(Box(render_box.top_left, self.shape.pixels.width, self.shape.pixels.height))
end
