using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace BnnyUi
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            richTextBox1.Clear();
        }

        private void ShowNotification(string message, string title,
                             MessageBoxButtons buttons, MessageBoxIcon icon)
        {
            MessageBox.Show(this, message, title, buttons, icon);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                var applicationBase = AppDomain.CurrentDomain.BaseDirectory;
                var targetDirectory = System.IO.Path.Combine(applicationBase, "bin");

                if (!System.IO.Directory.Exists(targetDirectory))
                {
                    ShowNotification("Required directory could not be located", "Directory Missing",
                                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }


                var discoveredFiles = System.IO.Directory.EnumerateFiles(targetDirectory,
                                        "bnny1.0.exe",
                                        System.IO.SearchOption.AllDirectories);

                string targetExecutable = discoveredFiles.FirstOrDefault();

                if (string.IsNullOrEmpty(targetExecutable))
                {
                    ShowNotification("Target executable is not present in the directory",
                                    "File Not Found", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    return;
                }

     
                var processConfiguration = new System.Diagnostics.ProcessStartInfo
                {
                    FileName = targetExecutable,
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden
                };

                using (var backgroundProcess = new System.Diagnostics.Process())
                {
                    backgroundProcess.StartInfo = processConfiguration;
                    backgroundProcess.Start();
                }
            }
            catch (System.IO.IOException ioEx)
            {
                ShowNotification($"File access issue: {ioEx.Message}", "IO Error",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (System.ComponentModel.Win32Exception win32Ex)
            {
                ShowNotification($"Execution failed: {win32Ex.Message}", "System Error",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (System.Exception generalEx)
            {
                ShowNotification($"Unexpected error: {generalEx.Message}", "Error",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
    }

        private void button3_Click(object sender, EventArgs e)
        {
            try
            {
                using (var networkClient = new System.Net.Sockets.TcpClient())
                {
                    var connectionResult = networkClient.BeginConnect("127.0.0.1", 6969, null, null);
                    var success = connectionResult.AsyncWaitHandle.WaitOne(TimeSpan.FromSeconds(3));

                    if (!success)
                    {
                        networkClient.Close();
                        ShowNotification("Connection timeout occurred", "Network Issue",
                                          MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        return;
                    }

                    networkClient.EndConnect(connectionResult);

                    using (var dataStream = networkClient.GetStream())
                    {
                        string scriptContent = richTextBox1.Text;
                        byte[] contentBuffer = System.Text.Encoding.UTF8.GetBytes(scriptContent);
                        int contentSize = contentBuffer.Length;

                        byte[] sizePrefix = BitConverter.GetBytes(contentSize);

                        // Ensure network byte order
                        if (BitConverter.IsLittleEndian)
                            Array.Reverse(sizePrefix);

                        // Write data with different approach
                        dataStream.Write(sizePrefix, 0, sizePrefix.Length);

                        int bytesWritten = 0;
                        while (bytesWritten < contentBuffer.Length)
                        {
                            int remaining = contentBuffer.Length - bytesWritten;
                            int chunkSize = Math.Min(1024, remaining);
                            dataStream.Write(contentBuffer, bytesWritten, chunkSize);
                            bytesWritten += chunkSize;
                        }

                        dataStream.Flush();
                    }
                }
            }
            catch (System.Net.Sockets.SocketException sockEx)
            {
                ShowNotification($"Network communication failed: {sockEx.Message}",
                                   "Connection Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (System.IO.IOException ioEx)
            {
                ShowNotification($"Data transmission issue: {ioEx.Message}",
                                   "IO Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (System.ObjectDisposedException disposedEx)
            {
                ShowNotification($"Resource already closed: {disposedEx.Message}",
                                   "Resource Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (System.Exception generalEx)
            {
                ShowNotification($"Operation failed: {generalEx.Message}",
                                   "System Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

    }
}
