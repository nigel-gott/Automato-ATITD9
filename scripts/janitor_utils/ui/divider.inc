dofile("janitor_utils/util/class.inc")
dofile("janitor_utils/util/vector.inc")
dofile("janitor_utils/util/box.inc")
dofile("janitor_utils/ui/render_result.inc")
dofile("janitor_utils/ui/colours.inc")

Divider = class{class_name="Divider"}

function Divider:init()
end

function Divider:render(render_box)
    lsDisplaySystemSprite(1, render_box.x, render_box.y, render_box.z, render_box.width, 5, WHITE)
    return RenderResult(Box(render_box.top_left, render_box.width, 5))
end

