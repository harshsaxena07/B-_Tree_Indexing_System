import tkinter as tk
from tkinter import messagebox
from tkinter import simpledialog
import subprocess

class BPlusTreeApp:
    def __init__(self, root):
        self.root = root
        self.root.title("TreeMendous_Four")
        
        # Set background color to black
        self.root.config(bg='black')

        # Define the labels and entry fields for various operations
        self.create_widgets()

    def create_widgets(self):
        # Set larger font size and white text color
        tk.Label(self.root, text="DATA_FETCHER USING B+ TREE", font=('Arial', 16, 'bold'), fg='white', bg='black').grid(row=0, column=0, columnspan=2, pady=10, padx=10)

        font = ('Arial', 12)
        fg_color = 'white'
        
        # Create widgets for 
        tk.Label(self.root, text="Data File:", font=font, fg=fg_color, bg='black').grid(row=1, column=0, pady=10, padx=10, sticky="w")
        self.data_file_entry = tk.Entry(self.root, font=font, width=40)
        self.data_file_entry.grid(row=1, column=1, pady=10, padx=10)

        tk.Label(self.root, text="Index File:", font=font, fg=fg_color, bg='black').grid(row=2, column=0, pady=10, padx=10, sticky="w")
        self.index_file_entry = tk.Entry(self.root, font=font, width=40)
        self.index_file_entry.grid(row=2, column=1, pady=10, padx=10)

        tk.Label(self.root, text="Key Length:", font=font, fg=fg_color, bg='black').grid(row=3, column=0, pady=10, padx=10, sticky="w")
        self.key_length_entry = tk.Entry(self.root, font=font, width=40)
        self.key_length_entry.grid(row=3, column=1, pady=10, padx=10)

        # Buttons for different operations
        button_font = ('Arial', 12)
        tk.Button(self.root, text="Create Index", command=self.create_index, font=button_font, bg='#4CAF50', fg='white').grid(row=4, column=0, columnspan=2, pady=15)
        tk.Button(self.root, text="Insert Record", command=self.insert_record, font=button_font, bg='#4CAF50', fg='white').grid(row=5, column=0, columnspan=2, pady=15)
        tk.Button(self.root, text="Find Record", command=self.find_record, font=button_font, bg='#4CAF50', fg='white').grid(row=6, column=0, columnspan=2, pady=15)
        tk.Button(self.root, text="List Records", command=self.list_records, font=button_font, bg='#4CAF50', fg='white').grid(row=7, column=0, columnspan=2, pady=15)

        # Output label just above the output box
        tk.Label(self.root, text="Output Result:", bg="black", fg="white", font=("Arial", 14, "bold")).grid(
            row=9, column=0, columnspan=2, padx=10, pady=(20, 0), sticky="w"
        )

        # Output text box
        self.result_text = tk.Text(self.root, width=50, height=10, font=('Arial', 12), wrap=tk.WORD)
        self.result_text.grid(row=10, column=0, columnspan=2, pady=(5, 20), padx=10)


        # Center the content
        self.center_window()

    def center_window(self):
        # screen width and height
        screen_width = self.root.winfo_screenwidth()
        screen_height = self.root.winfo_screenheight()

        # Define window content size (adjust as needed)
        window_width = 600  # Define the width for the content
        window_height = 500  # Define the height for the content

        # Calculate position to center the content
        position_top = int(screen_height / 2 - window_height / 2)
        position_left = int(screen_width / 2 - window_width / 2)

        # Set window size and position to keep it centered
        self.root.geometry(f'{window_width}x{window_height}+{position_left}+{position_top}')

        # Make window fullscreen but centered
        self.root.attributes('-fullscreen', True)
        self.root.after(1, lambda: self.root.attributes('-fullscreen', False))

    def create_index(self):
        data_file = self.data_file_entry.get()
        index_file = self.index_file_entry.get()
        key_length = int(self.key_length_entry.get())
        
        # Run the command to create the index
        command = f"btree.exe -create {data_file} {index_file} {key_length}"
        output = self.run_command(command)
        
        # Show result in text box
        if output:
            self.result_text.delete(1.0, tk.END)
            self.result_text.insert(tk.END, "Output Result:\n")
            self.result_text.insert(tk.END, output)

    def insert_record(self):
        index_file = self.index_file_entry.get()
        record = simpledialog.askstring("Input", "Enter record to insert (e.g., 230111565A ABHISHEK_KUMAR B-TECH DATA_SCIENCE I2 27):")
        if record:
            # Run the command to insert the record
            command = f'btree.exe -insert {index_file} "{record}"'
            output = self.run_command(command)
            
            # Show result in text box
            if output:
                self.result_text.delete(1.0, tk.END)
                self.result_text.insert(tk.END, "Output Result:\n")
                self.result_text.insert(tk.END, output)

    def find_record(self):
        index_file = self.index_file_entry.get()
        key = simpledialog.askstring("Input", "Enter key to find (e.g., 230111482A):")
        if key:
            key = key.strip()  # Remove extra spaces
            print(f"Searching for key: {key}")  # Debugging output
            command = f"btree.exe -find {index_file} {key}"
            output = self.run_command(command)
            
            # Show result in text box
            if not output:
                self.result_text.delete(1.0, tk.END)
                self.result_text.insert(tk.END, "Record not found.")
            else:
                self.result_text.delete(1.0, tk.END)
                self.result_text.insert(tk.END, output)

    def list_records(self):
        index_file = self.index_file_entry.get()
        if not index_file:
            messagebox.showerror("Input Error", "Please enter the index file name.")
            return
        index_file = index_file.strip()

        key = simpledialog.askstring("Input", "Enter starting key (e.g., 230111481A):")
        if not key:
            messagebox.showerror("Input Error", "Please enter a valid key.")
            return
        key = key.strip()

        count = simpledialog.askinteger("Input", "Enter number of records to list:")
        if not count or count <= 0:
            messagebox.showerror("Input Error", "Please enter a valid positive number for count.")
            return

        command = f"btree.exe -list {index_file} {key} {count}"
        print(f"Running command: {command}")  # Debug line

        output = self.run_command(command)

        # Show result in text box
        self.result_text.delete(1.0, tk.END)
        if output:
            self.result_text.insert(tk.END, output)
        else:
            self.result_text.insert(tk.END, "No records found or error occurred.")

    def run_command(self, command):
        try:
            # Run the C++ executable as a subprocess and capture output
            output = subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True, text=True)
            print(f"Command output: {output}")  # Debugging output
            return output
        except subprocess.CalledProcessError as e:
            print(f"Command error: {e.output}")  # Debugging output
            messagebox.showerror("Error", f"Error running command: {e.output}")
            return ""
        except FileNotFoundError:
            messagebox.showerror("Error", "Executable not found. Please ensure btree.exe is in the correct path.")
            return ""

# Run the Tkinter UI
if __name__ == "__main__":
    root = tk.Tk()
    app = BPlusTreeApp(root)
    root.mainloop()
