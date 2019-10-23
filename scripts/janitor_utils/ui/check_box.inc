dofile("janitor_utils/util/class.inc")
dofile("janitor_utils/util/vector.inc")
dofile("janitor_utils/util/box.inc")
dofile("janitor_utils/util/type_check.inc")
dofile("janitor_utils/ui/render_result.inc")
dofile("janitor_utils/ui/colours.inc")

CheckBox = class{class_name="CheckBox"}

function CheckBox:init(o)
    self.text = o.text
    self.colour = WHITE
    self.key = o.key
    self.value = readSetting(self.key, o.value)
end

function CheckBox:render(render_box)
    if render_box.height < 16 then
        error("CheckBox overflowed height")
    end
    self.value = lsCheckBox(render_box.x, render_box.y, render_box.z, self.colour, self.text, self.value);
    writeSetting(self.key, self.value)
    return RenderResult(Box(render_box.top_left, render_box.width, 16))
end
