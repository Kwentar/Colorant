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
using System.IO;

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

        private void pbtColorize_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog colorizeImageDialog = new OpenFileDialog();
            colorizeImageDialog.FileName = "Изображение";
            colorizeImageDialog.Filter = "Файлы изображений|*.jpg;*.bmp;*.png;*.jpeg";
            colorizeImageDialog.DefaultExt = ".jpg";
            Nullable<bool> resultDialog = colorizeImageDialog.ShowDialog();
            if (resultDialog == true)
            {
                FileInfo colorInfo;
                FileInfo grayInfo;
                FileInfo sourceInfo;
                string imageFileName = colorizeImageDialog.FileName;
                grayInfo = new FileInfo(imageFileName);
                try
                {
                    Process pro = Process.Start("..\\..\\..\\x64\\Debug\\MainColorMode.exe", "-colorize \"" + imageFileName + "\"");
                    pro.WaitForExit();
                    string path = Environment.CurrentDirectory;

                    colorInfo = new FileInfo(string.Format("{0}\\result.jpg", path));
                    sourceInfo = new FileInfo(string.Format("{0}\\source.jpg", path));
                    lbStatusLeft.Content = "Изображение успешно раскрашено";
                }
                catch
                {
                    string path = Environment.CurrentDirectory;
                    colorInfo = new FileInfo(string.Format("{0}\\errorOpen.png", path));
                    sourceInfo = new FileInfo(string.Format("{0}\\errorOpen.png", path));
                    lbStatusLeft.Content = "Не удалось открыть изображение";
                }
                MemoryStream msColorImage = new MemoryStream(System.IO.File.ReadAllBytes(colorInfo.FullName));
                BitmapImage colorImage = new BitmapImage();
                colorImage.BeginInit();
                colorImage.StreamSource = msColorImage;
                colorImage.EndInit();
                resultImg.Source = colorImage;

                MemoryStream msGreyImage = new MemoryStream(System.IO.File.ReadAllBytes(grayInfo.FullName));
                BitmapImage greyImage = new BitmapImage();
                greyImage.BeginInit();
                greyImage.StreamSource = msGreyImage;
                greyImage.EndInit();
                targetImg.Source = greyImage;

                MemoryStream msSourceImage = new MemoryStream(System.IO.File.ReadAllBytes(sourceInfo.FullName));
                BitmapImage sourceImage = new BitmapImage();
                sourceImage.BeginInit();
                sourceImage.StreamSource = msSourceImage;
                sourceImage.EndInit();
                sourceImg.Source = sourceImage;

            }
        }

        private void pbtViewBase_Click(object sender, RoutedEventArgs e)
        {
            ImageDataBase imageDataBaseForm = new ImageDataBase();
            imageDataBaseForm.Show();
        }
    }
}
