
dofile("janitor_utils/util/class.inc")
dofile("janitor_utils/algorithms/array_2d.inc")
dofile("janitor_utils/algorithms/shape.inc")
ShapeComparer = class{class_name = "ShapeComparer"}
function ShapeComparer:init()
end

function ShapeComparer:compare(target_shape, candidate_shape)
  -- Move in a spiral out from the anchor point of target, starting by overlaping anchor's and then moving the candidate anchor around
  local offset = candidate_shape.anchor_point - target_shape.anchor_point
  local max_offset = false
  local max_match = false
  for y=-10,10 do
    for x=-10,10 do
      local stepped_offset = offset + Point2D(offset.x +x, offset.y + y, offset.system)
      local num_matching_pixels = self:compare_with_offset(target_shape, candidate_shape, stepped_offset)
      if not max_offset or num_matching_pixels > max_match then
        max_offset = stepped_offset
        max_match = num_matching_pixels
      end
    end
  end
  print("Max " .. tostring(max_offset) .. " matching " .. max_match)
  return ComparisionResult{ matching_anchor = (target_shape.anchor_point + max_offset),
                            num_matching_pixels = max_match ,
                            matches = max_match >= (target_shape.pixels.size * 0.8) }
end

function ShapeComparer:compare_with_offset(target_shape, candidate_shape, offset)
  local num_matching_pixels = 0
  for xy, pixel in target_shape.pixels:iterate(false) do
    local offsetted = xy + offset
    local value = candidate_shape.pixels:get(offsetted)
    if value then
      num_matching_pixels = num_matching_pixels + 1
    end
  end
  return num_matching_pixels
end

ComparisionResult = class{class_name="ComparsionResult"}
function ComparisionResult:init(o)
  self.num_matching_pixels = o.num_matching_pixels
  self.matches = o.matches
  self.matching_anchor = o.matching_anchor
end


function test_shape_detector()
  local coord = CoordinateSystem{name="screen"}
  local target_shape = Shape(Array2D{
    offset = Point2D(10,10, coord),
    { 0, 0, 1, 1, 0, 0 },
    { 0, 1, 1, 1, 1, 0 },
    { 1, 1, 1, 1, 1, 1 },
    nil_value = 0,
    system = coord,
  })
  local matching_shape = Shape(Array2D{
    offset = Point2D(15,15, coord),
    { 0, 0, 1, 1, 0, 0 },
    { 0, 1, 1, 1, 1, 0 },
    { 1, 1, 1, 1, 1, 1 },
    nil_value = 0,
    system = coord,
  })
  local comparer = ShapeComparer()
  local matching_result = comparer:compare(target_shape, matching_shape)
  print(tostring(matching_result.matching_anchor))
  assert(matching_result.matches)
  assert(matching_result.num_matching_pixels == target_shape.pixels.size, matching_result.num_matching_pixels .. " != " .. target_shape.pixels.size)
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
