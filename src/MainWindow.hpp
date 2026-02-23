#pragma once

#include <string>

#include <gtkmm.h>

#include "RsyncRunner.hpp"

class MainWindow : public Gtk::Window {
public:
    explicit MainWindow(const std::string& icon_name);

private:
    void apply_css();
    void append_output(const std::string& text);
    void set_running_state(bool running);

    void on_sync_clicked();
    void on_progress(int percent);
    void on_runner_finished(int exit_code, bool signaled);

    bool confirm_synchronize();
    bool validate_inputs(std::string& origin, std::string& destination);
    void show_error(const std::string& message, const std::string& title = "Error");

    Gtk::Box vbox_{Gtk::ORIENTATION_VERTICAL};
    Gtk::Box top_hbox_{Gtk::ORIENTATION_HORIZONTAL};
    Gtk::Box directory_vbox_{Gtk::ORIENTATION_VERTICAL};
    Gtk::Box origin_hbox_{Gtk::ORIENTATION_HORIZONTAL};
    Gtk::Box destination_hbox_{Gtk::ORIENTATION_HORIZONTAL};
    Gtk::Box sync_button_vbox_{Gtk::ORIENTATION_VERTICAL};
    Gtk::Box middle_vbox_{Gtk::ORIENTATION_VERTICAL};
    Gtk::Box output_vbox_{Gtk::ORIENTATION_VERTICAL};
    Gtk::Box bottom_vbox_{Gtk::ORIENTATION_VERTICAL};

    Gtk::Label origin_label_{"Origin:"};
    Gtk::Label destination_label_{"Destination:"};

    Gtk::FileChooserButton origin_chooser_{"Select origin folder", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER};
    Gtk::FileChooserButton destination_chooser_{"Select destination folder", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER};

    Gtk::Button sync_button_{"Synchronize"};
    Gtk::ScrolledWindow output_scrolled_;
    Gtk::TextView output_view_;
    Glib::RefPtr<Gtk::TextBuffer> output_buffer_;
    Gtk::ProgressBar progress_bar_;

    RsyncRunner runner_;
    Glib::RefPtr<Gtk::CssProvider> css_provider_;
};
