
dofile("janitor_utils/util/class.inc")
ShapeComparer = class{class_name = "ShapeComparer"}
function ShapeComparer:init()
end

function ShapeComparer:compare(target_shape, candidate_shape)
  -- Move in a spiral out from the anchor point of target, starting by overlaping anchor's and then moving the candidate anchor around
  local num_matching_pixels = 0
  for y=1,target_shape.height do
    for x=1,target_shape.width do
      local pixel = target_shape.pixels[y] and target_shape.pixels[y][x]
      if pixel then
        if candidate_shape.pixels[y] and candidate_shape.pixels[y][x] then
          if pixel and pixel ~= 0 and pixel == candidate_shape[y][x] then
            num_matching_pixels = num_matching_pixels + 1
          end
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
