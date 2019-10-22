dofile("janitor_utils/util/class.inc")
dofile("janitor_utils/ui/colours.inc")
dofile("janitor_utils/algorithms/shape.inc")
dofile("janitor_utils/algorithms/screen_differ.inc")
dofile("janitor_utils/algorithms/pixel_connector.inc")

ScreenDifferWidget = class { class_name = "ScreenDifferWidget" }
function ScreenDifferWidget:init()
  self.state = "initial"
  local xyScreenSize = srGetWindowSize();
  self.differ = ScreenDiffer(Box(Vector(xyScreenSize[0] / 2 - xyScreenSize[0] / 8, xyScreenSize[1] / 2 - xyScreenSize[1] / 8, 0), xyScreenSize[0] / 4, xyScreenSize[1] / 4))
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
      self.pixel_connector = PixelConnector(
        self.differ.before_pixels,
        function(a, b)
          return self.differ:connected(a, b)
        end, function(pixel)
          return self.differ:changed(pixel)
        end)
      self.pixel_connector:connect()
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
    lsDisplaySystemSprite(1, render_box.x + xy.x, render_box.y + xy.y, render_box.z, 1, 1, RED);
  end
  return RenderResult(Box(render_box.top_left, self.shape.width, self.shape.height))
end
