import tkinter as tk
from tkinter import ttk, messagebox, scrolledtext, filedialog
import subprocess
import threading
import serial
import serial.tools.list_ports
from datetime import datetime
import time
import os
import sys

class VDUManager:
    def __init__(self, root):
        self.root = root
        self.root.title("VDU ESP32 Manager v1.0")
        self.root.geometry("900x700")
        self.root.configure(bg='#2b2b2b')
        
        # Variables
        self.serial_monitor_running = False
        self.serial_thread = None
        self.serial_connection = None
        # Update project path to parent directory (main VDU project)
        self.project_path = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        
        # Configure style
        self.setup_style()
        self.create_widgets()
        
    def setup_style(self):
        style = ttk.Style()
        style.theme_use('clam')
        
        # Configure colors
        style.configure('Title.TLabel', font=('Arial', 16, 'bold'), foreground='white', background='#2b2b2b')
        style.configure('Status.TLabel', font=('Arial', 10), foreground='white', background='#2b2b2b')
        
    def create_widgets(self):
        # Main frame
        main_frame = ttk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Title
        title_label = ttk.Label(main_frame, text="VDU ESP32 Manager", style='Title.TLabel')
        title_label.pack(pady=10)
        
        # Project info frame
        info_frame = ttk.LabelFrame(main_frame, text="Project Information")
        info_frame.pack(fill=tk.X, pady=5)
        
        ttk.Label(info_frame, text=f"Project Path: {self.project_path}").pack(anchor=tk.W, padx=5, pady=2)
        ttk.Label(info_frame, text="Hardware: ESP32 DevKit V1 | Display: 16x2 LCD | RTC: DS3231").pack(anchor=tk.W, padx=5, pady=2)
        
        # Control buttons frame
        control_frame = ttk.LabelFrame(main_frame, text="Controls")
        control_frame.pack(fill=tk.X, pady=5)
        
        # Buttons row 1
        btn_frame1 = ttk.Frame(control_frame)
        btn_frame1.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Button(btn_frame1, text="📤 Upload Firmware", 
                  command=self.upload_firmware).pack(side=tk.LEFT, padx=5)
        ttk.Button(btn_frame1, text="🔍 Build Only", 
                  command=self.build_firmware).pack(side=tk.LEFT, padx=5)
        ttk.Button(btn_frame1, text="🧹 Clean Build", 
                  command=self.clean_build).pack(side=tk.LEFT, padx=5)
        
        # Buttons row 2
        btn_frame2 = ttk.Frame(control_frame)
        btn_frame2.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Button(btn_frame2, text="📡 Start Monitor", 
                  command=self.start_monitor).pack(side=tk.LEFT, padx=5)
        ttk.Button(btn_frame2, text="⏹️ Stop Monitor", 
                  command=self.stop_monitor).pack(side=tk.LEFT, padx=5)
        ttk.Button(btn_frame2, text="🕐 Set RTC Time", 
                  command=self.set_rtc_time).pack(side=tk.LEFT, padx=5)
        ttk.Button(btn_frame2, text="🔄 Refresh Ports", 
                  command=self.refresh_ports).pack(side=tk.LEFT, padx=5)
        
        # Port selection frame
        port_frame = ttk.LabelFrame(main_frame, text="Serial Port Configuration")
        port_frame.pack(fill=tk.X, pady=5)
        
        port_select_frame = ttk.Frame(port_frame)
        port_select_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Label(port_select_frame, text="Port:").pack(side=tk.LEFT, padx=5)
        self.port_var = tk.StringVar()
        self.port_combo = ttk.Combobox(port_select_frame, textvariable=self.port_var, width=15)
        self.port_combo.pack(side=tk.LEFT, padx=5)
        
        ttk.Label(port_select_frame, text="Baud Rate:").pack(side=tk.LEFT, padx=5)
        self.baud_var = tk.StringVar(value="115200")
        baud_combo = ttk.Combobox(port_select_frame, textvariable=self.baud_var, 
                                 values=["9600", "19200", "38400", "57600", "115200"], width=10)
        baud_combo.pack(side=tk.LEFT, padx=5)
        
        self.refresh_ports()
        
        # Serial monitor frame
        monitor_frame = ttk.LabelFrame(main_frame, text="Serial Monitor")
        monitor_frame.pack(fill=tk.BOTH, expand=True, pady=5)
        
        # Monitor controls
        monitor_controls = ttk.Frame(monitor_frame)
        monitor_controls.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Button(monitor_controls, text="Clear", 
                  command=self.clear_monitor).pack(side=tk.LEFT, padx=5)
        ttk.Button(monitor_controls, text="Save Log", 
                  command=self.save_log).pack(side=tk.LEFT, padx=5)
        
        # Serial output
        self.serial_text = scrolledtext.ScrolledText(monitor_frame, height=20, 
                                                   bg='black', fg='#00ff00', font=("Consolas", 10))
        self.serial_text.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Status bar
        self.status_var = tk.StringVar()
        self.status_var.set("Ready")
        status_bar = ttk.Label(main_frame, textvariable=self.status_var, 
                              relief=tk.SUNKEN, anchor=tk.W, style='Status.TLabel')
        status_bar.pack(fill=tk.X, pady=5)
        
        # Auto-refresh ports
        self.root.after(5000, self.auto_refresh_ports)
        
    def refresh_ports(self):
        """Refresh available serial ports"""
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_combo['values'] = ports
        if ports and not self.port_var.get():
            self.port_var.set(ports[0])
            
    def auto_refresh_ports(self):
        """Auto-refresh ports every 5 seconds"""
        self.refresh_ports()
        self.root.after(5000, self.auto_refresh_ports)
        
    def upload_firmware(self):
        """Upload firmware to ESP32"""
        def upload():
            self.status_var.set("Uploading firmware...")
            self.log_message("Starting firmware upload...")
            
            try:
                result = subprocess.run(['pio', 'run', '--target', 'upload'], 
                                      capture_output=True, text=True, timeout=120, cwd=self.project_path)
                
                if result.returncode == 0:
                    self.log_message("✅ Firmware uploaded successfully!")
                    messagebox.showinfo("Success", "Firmware uploaded successfully!")
                else:
                    error_msg = f"❌ Upload failed:\n{result.stderr}"
                    self.log_message(error_msg)
                    messagebox.showerror("Error", error_msg)
                    
            except subprocess.TimeoutExpired:
                error_msg = "❌ Upload timeout - check connection"
                self.log_message(error_msg)
                messagebox.showerror("Error", error_msg)
            except FileNotFoundError:
                error_msg = "❌ PlatformIO not found - install PlatformIO CLI"
                self.log_message(error_msg)
                messagebox.showerror("Error", error_msg)
            except Exception as e:
                error_msg = f"❌ Upload error: {str(e)}"
                self.log_message(error_msg)
                messagebox.showerror("Error", error_msg)
            finally:
                self.status_var.set("Ready")
        
        threading.Thread(target=upload, daemon=True).start()
        
    def build_firmware(self):
        """Build firmware without uploading"""
        def build():
            self.status_var.set("Building firmware...")
            self.log_message("Starting firmware build...")
            
            try:
                result = subprocess.run(['pio', 'run'], 
                                      capture_output=True, text=True, timeout=60, cwd=self.project_path)
                
                if result.returncode == 0:
                    self.log_message("✅ Firmware built successfully!")
                    messagebox.showinfo("Success", "Firmware built successfully!")
                else:
                    error_msg = f"❌ Build failed:\n{result.stderr}"
                    self.log_message(error_msg)
                    messagebox.showerror("Error", error_msg)
                    
            except Exception as e:
                error_msg = f"❌ Build error: {str(e)}"
                self.log_message(error_msg)
                messagebox.showerror("Error", error_msg)
            finally:
                self.status_var.set("Ready")
        
        threading.Thread(target=build, daemon=True).start()
        
    def clean_build(self):
        """Clean and rebuild firmware"""
        def clean():
            self.status_var.set("Cleaning build...")
            self.log_message("Cleaning build files...")
            
            try:
                result = subprocess.run(['pio', 'run', '--target', 'clean'], 
                                      capture_output=True, text=True, timeout=30, cwd=self.project_path)
                
                if result.returncode == 0:
                    self.log_message("✅ Build cleaned successfully!")
                    messagebox.showinfo("Success", "Build cleaned successfully!")
                else:
                    error_msg = f"❌ Clean failed:\n{result.stderr}"
                    self.log_message(error_msg)
                    messagebox.showerror("Error", error_msg)
                    
            except Exception as e:
                error_msg = f"❌ Clean error: {str(e)}"
                self.log_message(error_msg)
                messagebox.showerror("Error", error_msg)
            finally:
                self.status_var.set("Ready")
        
        threading.Thread(target=clean, daemon=True).start()
        
    def start_monitor(self):
        """Start serial monitor"""
        if self.serial_monitor_running:
            return
            
        port = self.port_var.get()
        if not port:
            messagebox.showerror("Error", "Please select a serial port!")
            return
            
        self.serial_monitor_running = True
        self.serial_thread = threading.Thread(target=self.monitor_serial, daemon=True)
        self.serial_thread.start()
        self.status_var.set("Serial monitor running...")
        self.log_message(f"📡 Started monitoring {port}")
        
    def stop_monitor(self):
        """Stop serial monitor"""
        self.serial_monitor_running = False
        if self.serial_connection:
            self.serial_connection.close()
        self.status_var.set("Serial monitor stopped")
        self.log_message("⏹️ Serial monitor stopped")
        
    def monitor_serial(self):
        """Monitor serial port"""
        try:
            baud_rate = int(self.baud_var.get())
            self.serial_connection = serial.Serial(self.port_var.get(), baud_rate, timeout=1)
            
            while self.serial_monitor_running:
                if self.serial_connection.in_waiting:
                    data = self.serial_connection.readline().decode('utf-8', errors='ignore')
                    self.log_message(data.strip())
                time.sleep(0.1)
                
        except Exception as e:
            self.log_message(f"❌ Serial monitor error: {str(e)}")
            messagebox.showerror("Error", f"Serial monitor error: {str(e)}")
        finally:
            if self.serial_connection:
                self.serial_connection.close()
            
    def clear_monitor(self):
        """Clear serial monitor"""
        self.serial_text.delete(1.0, tk.END)
        
    def save_log(self):
        """Save serial log to file"""
        filename = filedialog.asksaveasfilename(
            defaultextension=".txt",
            filetypes=[("Text files", "*.txt"), ("All files", "*.*")],
            title="Save Serial Log"
        )
        
        if filename:
            try:
                with open(filename, 'w') as f:
                    f.write(self.serial_text.get(1.0, tk.END))
                messagebox.showinfo("Success", f"Log saved to {filename}")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to save log: {str(e)}")
                
    def log_message(self, message):
        """Add message to serial monitor"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        self.serial_text.insert(tk.END, f"[{timestamp}] {message}\n")
        self.serial_text.see(tk.END)
        
    def set_rtc_time(self):
        """Set RTC time dialog"""
        dialog = tk.Toplevel(self.root)
        dialog.title("Set RTC Time")
        dialog.geometry("400x350")
        dialog.transient(self.root)
        dialog.grab_set()
        dialog.configure(bg='#2b2b2b')
        
        # Center dialog
        dialog.update_idletasks()
        x = (dialog.winfo_screenwidth() // 2) - (400 // 2)
        y = (dialog.winfo_screenheight() // 2) - (350 // 2)
        dialog.geometry(f"400x350+{x}+{y}")
        
        # Title
        title_label = ttk.Label(dialog, text="Set RTC Time", style='Title.TLabel')
        title_label.pack(pady=10)
        
        # Current time frame
        current_frame = ttk.LabelFrame(dialog, text="Current System Time")
        current_frame.pack(fill=tk.X, padx=10, pady=5)
        
        now = datetime.now()
        ttk.Label(current_frame, text=f"System: {now.strftime('%Y-%m-%d %H:%M:%S')}").pack(pady=5)
        
        # Time entry frame
        time_frame = ttk.LabelFrame(dialog, text="Set RTC Time")
        time_frame.pack(fill=tk.X, padx=10, pady=5)
        
        # Time entries
        entries_frame = ttk.Frame(time_frame)
        entries_frame.pack(pady=10)
        
        # Hours
        ttk.Label(entries_frame, text="Hours:").grid(row=0, column=0, padx=5, pady=5)
        hours_var = tk.StringVar(value=str(now.hour))
        hours_entry = ttk.Entry(entries_frame, textvariable=hours_var, width=5)
        hours_entry.grid(row=0, column=1, padx=5, pady=5)
        
        # Minutes
        ttk.Label(entries_frame, text="Minutes:").grid(row=0, column=2, padx=5, pady=5)
        minutes_var = tk.StringVar(value=str(now.minute))
        minutes_entry = ttk.Entry(entries_frame, textvariable=minutes_var, width=5)
        minutes_entry.grid(row=0, column=3, padx=5, pady=5)
        
        # Seconds
        ttk.Label(entries_frame, text="Seconds:").grid(row=0, column=4, padx=5, pady=5)
        seconds_var = tk.StringVar(value=str(now.second))
        seconds_entry = ttk.Entry(entries_frame, textvariable=seconds_var, width=5)
        seconds_entry.grid(row=0, column=5, padx=5, pady=5)
        
        # Date entries
        date_frame = ttk.Frame(time_frame)
        date_frame.pack(pady=10)
        
        # Day
        ttk.Label(date_frame, text="Day:").grid(row=0, column=0, padx=5, pady=5)
        day_var = tk.StringVar(value=str(now.day))
        day_entry = ttk.Entry(date_frame, textvariable=day_var, width=5)
        day_entry.grid(row=0, column=1, padx=5, pady=5)
        
        # Month
        ttk.Label(date_frame, text="Month:").grid(row=0, column=2, padx=5, pady=5)
        month_var = tk.StringVar(value=str(now.month))
        month_entry = ttk.Entry(date_frame, textvariable=month_var, width=5)
        month_entry.grid(row=0, column=3, padx=5, pady=5)
        
        # Year
        ttk.Label(date_frame, text="Year:").grid(row=0, column=4, padx=5, pady=5)
        year_var = tk.StringVar(value=str(now.year))
        year_entry = ttk.Entry(date_frame, textvariable=year_var, width=5)
        year_entry.grid(row=0, column=5, padx=5, pady=5)
        
        # Buttons
        btn_frame = ttk.Frame(dialog)
        btn_frame.pack(pady=20)
        
        def apply_time():
            try:
                # Validate inputs
                hours = int(hours_var.get())
                minutes = int(minutes_var.get())
                seconds = int(seconds_var.get())
                day = int(day_var.get())
                month = int(month_var.get())
                year = int(year_var.get())
                
                # Validate ranges
                if not (0 <= hours <= 23 and 0 <= minutes <= 59 and 0 <= seconds <= 59):
                    raise ValueError("Invalid time values")
                if not (1 <= day <= 31 and 1 <= month <= 12 and 2020 <= year <= 2030):
                    raise ValueError("Invalid date values")
                
                # Send time update command via serial
                port = self.port_var.get()
                if not port:
                    messagebox.showerror("Error", "Please select a serial port first!")
                    return
                
                # Check if monitor is running - if not, start it first
                if not self.serial_monitor_running:
                    messagebox.showwarning("Warning", "Serial monitor is not running. Starting monitor first...")
                    self.start_monitor()
                    time.sleep(2)  # Wait for monitor to start
                
                try:
                    # Use the existing serial connection from the monitor
                    if not self.serial_connection or not self.serial_connection.is_open:
                        messagebox.showerror("Error", "Serial connection not available. Please start the monitor first.")
                        return
                    
                    ser = self.serial_connection
                    
                    # Send time update command exactly like serial monitor
                    time_command = f"SET_TIME {year:04d} {month:02d} {day:02d} {hours:02d} {minutes:02d} {seconds:02d}\n"
                    ser.write(time_command.encode())
                    
                    # Small delay to ensure ESP32 processes the command
                    time.sleep(0.5)
                    
                    # Wait for response (read multiple lines to get the actual response)
                    response_lines = []
                    start_time = time.time()
                    timeout = 3.0  # 3 second timeout
                        
                    while time.time() - start_time < timeout:
                        try:
                            if ser.in_waiting:
                                line = ser.readline().decode('utf-8', errors='ignore').strip()
                                if line:
                                    response_lines.append(line)
                                    # Check if we got the success message
                                    if "RTC time set successfully" in line:
                                        self.log_message(f"🕐 RTC time set successfully: {day:02d}/{month:02d}/{year} {hours:02d}:{minutes:02d}:{seconds:02d}")
                                        messagebox.showinfo("Success", f"RTC time updated successfully!\n{day:02d}/{month:02d}/{year} {hours:02d}:{minutes:02d}:{seconds:02d}")
                                        return
                                    # Check if we got an error message
                                    elif "Failed to set RTC time" in line:
                                        self.log_message(f"❌ RTC time set failed: {line}")
                                        messagebox.showerror("Error", f"Failed to set RTC time:\n{line}")
                                        return
                            time.sleep(0.1)
                        except:
                            break
                    
                    # If we didn't get a clear response, check if any line contains success indicators
                    success_indicators = ["RTC time set successfully", "OK", "SUCCESS"]
                    error_indicators = ["Failed to set RTC time", "ERROR", "FAILED"]
                    
                    success_found = False
                    error_found = False
                    for line in response_lines:
                        for indicator in success_indicators:
                            if indicator in line:
                                success_found = True
                                break
                        for indicator in error_indicators:
                            if indicator in line:
                                error_found = True
                                break
                    
                    if success_found:
                        self.log_message(f"🕐 RTC time set successfully: {day:02d}/{month:02d}/{year} {hours:02d}:{minutes:02d}:{seconds:02d}")
                        messagebox.showinfo("Success", f"RTC time updated successfully!\n{day:02d}/{month:02d}/{year} {hours:02d}:{minutes:02d}:{seconds:02d}")
                    elif error_found:
                        response_text = "\n".join(response_lines) if response_lines else "No response"
                        self.log_message(f"❌ RTC time set failed: {response_text}")
                        messagebox.showerror("Error", f"Failed to set RTC time:\n{response_text}")
                    else:
                        # Log all response lines for debugging
                        response_text = "\n".join(response_lines) if response_lines else "No response"
                        self.log_message(f"⚠️ RTC time update response: {response_text}")
                        messagebox.showwarning("Warning", f"Time update sent, but response unclear:\n{response_text}")
                    
                    # Ask user if they want to restart the monitor
                    if messagebox.askyesno("Restart Monitor", "Do you want to restart the serial monitor?"):
                        self.start_monitor()
                            
                except Exception as e:
                    error_msg = f"Failed to send time update: {str(e)}"
                    self.log_message(f"❌ {error_msg}")
                    messagebox.showerror("Error", error_msg)
                
                dialog.destroy()
                
            except ValueError as e:
                messagebox.showerror("Error", f"Invalid values: {str(e)}")
        
        ttk.Button(btn_frame, text="Apply", command=apply_time).pack(side=tk.LEFT, padx=10)
        ttk.Button(btn_frame, text="Cancel", command=dialog.destroy).pack(side=tk.LEFT, padx=10)

def main():
    root = tk.Tk()
    app = VDUManager(root)
    
    # Center window
    root.update_idletasks()
    x = (root.winfo_screenwidth() // 2) - (900 // 2)
    y = (root.winfo_screenheight() // 2) - (700 // 2)
    root.geometry(f"900x700+{x}+{y}")
    
    root.mainloop()

if __name__ == "__main__":
    main() 