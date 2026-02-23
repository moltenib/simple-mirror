#include "MainWindow.hpp"

#include <filesystem>
#include <sstream>

namespace {

constexpr const char* kAppCss = R"CSS(
#main-window #main-box {
  background: linear-gradient(180deg, #0b0b0b, #160707);
  padding: 12px;
}

#main-window #output-view {
  background: #000000;
  color: #ffffff;
  border-radius: 8px;
  border: 1px solid #ffffff;
}

#main-window #origin-label,
#main-window #destination-label {
  color: #ffd6d6;
}

#main-window #origin-chooser,
#main-window #destination-chooser,
#main-window #sync-button {
  color: #ffe7e7;
}

#main-window #sync-button,
#main-window #origin-chooser,
#main-window #destination-chooser {
  background-image: none;
  background-color: #2a0d0d;
  border: 1px solid #6f2020;
  border-radius: 6px;
}

#main-window #progress-bar trough {
  min-height: 10px;
  background-color: #1b1b1b;
  border: 1px solid #4a1414;
  border-radius: 6px;
}

#main-window #progress-bar progress {
  background-image: none;
  background-color: #cc1f1f;
  border-radius: 6px;
}

#main-window #sync-button {
  font-weight: 700;
  background-color: #b51616;
  border: 1px solid #ff4d4d;
  color: #fff1f1;
}

#main-window #header-bar {
  background-image: none;
  background-color: #180909;
  border-bottom: 1px solid #4a1414;
}

#main-window #header-bar .title,
#main-window #header-bar .subtitle {
  color: #ffd6d6;
}
)CSS";

}  // namespace

MainWindow::MainWindow(const std::string& icon_name)
    : Gtk::Window(Gtk::WINDOW_TOPLEVEL) {
    set_title("Quick Back-Up");
    set_name("main-window");
    set_default_size(800, 600);
    set_size_request(800, 600);
    set_position(Gtk::WIN_POS_CENTER);
    set_border_width(0);

    if (!icon_name.empty()) {
        set_icon_name(icon_name);
    }

    auto* header = Gtk::make_managed<Gtk::HeaderBar>();
    header->set_name("header-bar");
    header->set_show_close_button(true);
    header->set_title("Quick Back-Up");
    set_titlebar(*header);

    vbox_.set_name("main-box");
    add(vbox_);

    vbox_.pack_start(top_hbox_, Gtk::PACK_SHRINK, 0);
    top_hbox_.pack_start(directory_vbox_, Gtk::PACK_EXPAND_WIDGET, 6);

    directory_vbox_.pack_start(origin_hbox_, Gtk::PACK_SHRINK, 6);
    origin_label_.set_name("origin-label");
    origin_label_.set_xalign(0.0f);
    origin_hbox_.pack_start(origin_label_, Gtk::PACK_SHRINK, 6);

    origin_chooser_.set_name("origin-chooser");
    origin_hbox_.pack_start(origin_chooser_, Gtk::PACK_EXPAND_WIDGET, 6);

    directory_vbox_.pack_start(destination_hbox_, Gtk::PACK_SHRINK, 6);
    destination_label_.set_name("destination-label");
    destination_label_.set_xalign(0.0f);
    destination_hbox_.pack_start(destination_label_, Gtk::PACK_SHRINK, 6);

    destination_chooser_.set_name("destination-chooser");
    destination_hbox_.pack_start(destination_chooser_, Gtk::PACK_EXPAND_WIDGET, 6);

    top_hbox_.pack_start(sync_button_vbox_, Gtk::PACK_SHRINK, 6);
    sync_button_.set_name("sync-button");
    sync_button_.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_sync_clicked));
    sync_button_vbox_.pack_end(sync_button_, Gtk::PACK_EXPAND_WIDGET, 6);

    vbox_.pack_start(middle_vbox_, Gtk::PACK_EXPAND_WIDGET, 0);
    middle_vbox_.pack_start(output_vbox_, Gtk::PACK_EXPAND_WIDGET, 6);

    output_scrolled_.set_hexpand(true);
    output_scrolled_.set_vexpand(true);
    output_scrolled_.set_shadow_type(Gtk::SHADOW_NONE);
    output_vbox_.pack_start(output_scrolled_, Gtk::PACK_EXPAND_WIDGET, 6);

    output_view_.set_name("output-view");
    output_view_.set_editable(false);
    output_view_.set_cursor_visible(false);
    output_view_.set_monospace(true);

    output_buffer_ = Gtk::TextBuffer::create();
    output_view_.set_buffer(output_buffer_);
    output_scrolled_.add(output_view_);

    vbox_.pack_end(bottom_vbox_, Gtk::PACK_SHRINK, 0);
    progress_bar_.set_name("progress-bar");
    progress_bar_.set_show_text(true);
    progress_bar_.set_text("Idle");
    bottom_vbox_.pack_end(progress_bar_, Gtk::PACK_SHRINK, 6);

    runner_.set_output_callback([this](const std::string& line) { append_output(line); });
    runner_.set_progress_callback([this](int percent) { on_progress(percent); });
    runner_.set_finished_callback([this](int exit_code, bool signaled) {
        on_runner_finished(exit_code, signaled);
    });

    apply_css();
    show_all_children();
}

void MainWindow::apply_css() {
    css_provider_ = Gtk::CssProvider::create();
    css_provider_->load_from_data(kAppCss);

    auto screen = Gdk::Screen::get_default();
    if (screen) {
        Gtk::StyleContext::add_provider_for_screen(
            screen,
            css_provider_,
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }
}

void MainWindow::append_output(const std::string& text) {
    auto end_iter = output_buffer_->end();
    output_buffer_->insert(end_iter, text);

    auto mark = output_buffer_->create_mark(output_buffer_->end());
    output_view_.scroll_to(mark);
}

void MainWindow::set_running_state(bool running) {
    sync_button_.set_sensitive(true);
    sync_button_.set_label(running ? "Cancel" : "Synchronize");
    origin_chooser_.set_sensitive(!running);
    destination_chooser_.set_sensitive(!running);

    if (running) {
        progress_bar_.set_fraction(0.0);
        progress_bar_.set_text("Running...");
    }
}

void MainWindow::on_sync_clicked() {
    if (runner_.is_running()) {
        runner_.cancel();
        progress_bar_.set_text("Stopping...");
        return;
    }

    std::string origin;
    std::string destination;
    if (!validate_inputs(origin, destination)) {
        return;
    }

    std::string rsync_error;
    if (!runner_.ensure_rsync_available(rsync_error)) {
        show_error(rsync_error, "rsync not found");
        return;
    }

    if (!confirm_synchronize()) {
        return;
    }

    if (output_buffer_) {
        output_buffer_->set_text("");
    }

    set_running_state(true);

    append_output("$ rsync -aP --delete \"" + origin + "\" \"" + destination + "\"\n\n");

    std::string error;
    if (!runner_.start(origin, destination, error)) {
        set_running_state(false);
        progress_bar_.set_text("Error");
        append_output("Error: " + error + "\n");
        show_error(error, "Synchronization error");
    }
}

void MainWindow::on_progress(int percent) {
    progress_bar_.set_fraction(static_cast<double>(percent) / 100.0);
    progress_bar_.set_text(std::to_string(percent) + "%");
}

void MainWindow::on_runner_finished(int exit_code, bool signaled) {
    if (signaled) {
        progress_bar_.set_text("Cancelled");
    } else if (exit_code == 0) {
        progress_bar_.set_fraction(1.0);
        progress_bar_.set_text("Done");
    } else {
        progress_bar_.set_text("Failed");
        show_error(
            "Synchronization failed with exit code " + std::to_string(exit_code) + ".",
            "Synchronization failed");
    }

    set_running_state(false);
}

bool MainWindow::confirm_synchronize() {
    Gtk::MessageDialog dialog(
        *this,
        "Notice",
        false,
        Gtk::MESSAGE_WARNING,
        Gtk::BUTTONS_OK_CANCEL,
        true);

    dialog.set_secondary_text(
        "Any files in the destination folder that do not exist in the origin will be deleted. "
        "This is to keep the destination folder up to date. Continue?");

    return dialog.run() == Gtk::RESPONSE_OK;
}

bool MainWindow::validate_inputs(std::string& origin, std::string& destination) {
    origin = origin_chooser_.get_filename();
    destination = destination_chooser_.get_filename();

    if (origin.empty() || destination.empty()) {
        show_error("Please choose both origin and destination folders.");
        return false;
    }

    if (!std::filesystem::is_directory(origin)) {
        show_error("Origin folder does not exist: " + origin);
        return false;
    }

    if (!std::filesystem::is_directory(destination)) {
        show_error("Destination folder does not exist: " + destination);
        return false;
    }

    if (origin.back() != '/') {
        origin.push_back('/');
    }
    if (destination.back() != '/') {
        destination.push_back('/');
    }

    return true;
}

void MainWindow::show_error(const std::string& message, const std::string& title) {
    Gtk::MessageDialog dialog(
        *this,
        title,
        false,
        Gtk::MESSAGE_ERROR,
        Gtk::BUTTONS_CLOSE,
        true);
    dialog.set_secondary_text(message);
    dialog.run();
}
