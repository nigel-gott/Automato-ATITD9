dofile("janitor_utils/util/class.inc")

function test_screen_differ()
end
ScreenDiffer = class { class_name = "ScreenDiffer" }
function ScreenDiffer:init(box)
  self.box = box
end
function ScreenDiffer:snapshot()
  srReadScreen()
end
function ScreenDiffer:snapshot_and_store()
  self.before_pixels = {}
  srReadScreen()
  for y=1,self.box.height do
    self.before_pixels[y] = {}
    for x=1,self.box.width do
      self.before_pixels[y][x] = srReadPixelFromBuffer(self.box.x + x, self.box.y + y)
    end
  end
end

function ScreenDiffer:changed(pixel)
  local new = srReadPixelFromBuffer(pixel.x + self.box.x, pixel.y + self.box.y)
  return new ~= self.before_pixels[pixel.y][pixel.x]
end

