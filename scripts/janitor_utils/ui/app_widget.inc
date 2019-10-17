dofile("janitor_utils/util/class.inc")
dofile("janitor_utils/util/vector.inc")
dofile("common.inc")

AppWidget = class { class_name = "AppWidget" }

function AppWidget:init(child, config)
    assert(child, "Must pass child to AppWidget constructor")
    self.child = child
    self.per_frame_sleep_time = 10
end

function AppWidget:run()
    local window_size = lsGetWindowSize()
    self.child:render(Box(Vector(0, 0, 1), window_size[0], window_size[1]))
    lsSleep(self.per_frame_sleep_time)
    checkBreak()
    lsDoFrame()
    return JobRunning{}
end

