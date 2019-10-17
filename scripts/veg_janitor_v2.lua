cleanups = {}

dofile("janitor_utils/util/table_printer.inc")
dofile("janitor_utils/util/string.inc")
dofile("janitor_utils/ui/widgets.inc")
dofile("janitor_utils/ui/pages_widget.inc")
dofile("janitor_utils/ui/colours.inc")

dofile("janitor_utils/algorithms/screen_differ_widget.inc")
dofile("janitor_utils/jobs/job_manager.inc")
dofile("janitor_utils/jobs/window_manager.inc")
dofile("janitor_utils/jobs/click_manager.inc")
dofile("janitor_utils/jobs/job_manager_widget.inc")

function doit()
    askForWindow("Press shift over the ATITD window to begin")
    local job_manager = JobManager()
    local click_manager = ClickManager()
    local window_manager = WindowManager(click_manager)

    local app = AppWidget(Padding {
        all = 10,
        child = PagesWidget({
          { name = "Differ", widget = ScreenDifferWidget() },
          { name = "JobStats", widget = JobManagerWidget(job_manager) },
        },
                "JobStats",
                {
                    Button {
                        text = "Exit",
                        on_pressed = function()
                            job_manager:should_exit()
                        end,
                        colour = RED
                    },
                })
    }, config)

    job_manager:submit {
        job = app,
        name = "app"
    }
    job_manager:run()

end
