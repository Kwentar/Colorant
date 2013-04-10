using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Microsoft.Win32;
using System.Diagnostics;

namespace ColorInterface
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        public MainWindow()
        {
            InitializeComponent();
        }

        private void pbtAddImage_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog addImageDialog = new OpenFileDialog();
            addImageDialog.FileName = "Изображение";
            addImageDialog.Filter = "Файлы изображений|*.jpg;*.bmp;*.png;*.jpeg";
            addImageDialog.DefaultExt = ".jpg";
            Nullable<bool> resultDialog=addImageDialog.ShowDialog();
            if (resultDialog==true)
            {
                string imageFileName=addImageDialog.FileName;
                Uri uriImage = new Uri(imageFileName);
                BitmapImage addsImage;
                try
                {
                    addsImage = new BitmapImage(uriImage);
                    Process pro = Process.Start("..\\..\\..\\x64\\Debug\\MainColorMode.exe", "-add \"" + imageFileName + "\"");
                    pro.WaitForExit();
                    lbStatusLeft.Content = "Изображение успешно добавлено";
                }
                catch 
                {
                    string path = Environment.CurrentDirectory;
                    addsImage = new BitmapImage(new Uri(string.Format("{0}\\errorOpen.png",path)));
                    lbStatusLeft.Content = "Не удалось открыть изображение";
                }
                sourceImg.Source = addsImage;
            }
        }

        private void pbtAddCatalog_Click(object sender, RoutedEventArgs e)
        {

        }

        private void pbtColorize_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog colorizeImageDialog = new OpenFileDialog();
            colorizeImageDialog.FileName = "Изображение";
            colorizeImageDialog.Filter = "Файлы изображений|*.jpg;*.bmp;*.png;*.jpeg";
            colorizeImageDialog.DefaultExt = ".jpg";
            Nullable<bool> resultDialog = colorizeImageDialog.ShowDialog();
            if (resultDialog == true)
            {
                string imageFileName = colorizeImageDialog.FileName;
                Uri uriImage = new Uri(imageFileName);
                BitmapImage colorizeImage;
                try
                {
                    Process pro = Process.Start("..\\..\\..\\x64\\Debug\\MainColorMode.exe", "-colorize \"" + imageFileName + "\"");
                    pro.WaitForExit();
                    string path = Environment.CurrentDirectory;
                    colorizeImage = new BitmapImage(new Uri(string.Format("{0}\\result.jpg", path)));
                    lbStatusLeft.Content = "Изображение успешно раскрашено";
                }
                catch
                {
                    string path = Environment.CurrentDirectory;
                    colorizeImage = new BitmapImage(new Uri(string.Format("{0}\\errorOpen.png", path)));
                    lbStatusLeft.Content = "Не удалось открыть изображение";
                }
                resultImg.Source = colorizeImage;
            }
        }
    }
}
