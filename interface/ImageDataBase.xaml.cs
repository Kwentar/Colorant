using System;
using System.IO;
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
using System.Windows.Shapes;
using System.Diagnostics;
using System.Windows.Forms;

namespace ColorInterface
{
    /// <summary>
    /// Interaction logic for ImageDataBase.xaml
    /// </summary>
    public partial class ImageDataBase : Window
    {
        List<DBImage> images = new List<DBImage>();
        private int currImage = 0;
        private int countImages = 0;
        BitmapImage noImage = new BitmapImage(new Uri(Environment.CurrentDirectory + "\\noImage.png"));
        public ImageDataBase()
        {
            InitializeComponent();
            getImages();
            this.Title = "База данных изображений. Количество изображений: " + countImages.ToString();
        }

        private void getImages()
        {
            try
            {
                images.Clear();
                string path = Environment.CurrentDirectory;
                countImages = 0;
                
                foreach(string fileName in Directory.EnumerateFiles(string.Format(@"{0}\base\",path)))
                {
                    if (fileName.Contains(".png"))
                    {
                        FileInfo t = new FileInfo(fileName);
                        MemoryStream tmp_img = new MemoryStream(File.ReadAllBytes(t.FullName));
                        BitmapImage ty = new BitmapImage();
                        ty.BeginInit();
                        ty.StreamSource = tmp_img;

                        ty.EndInit();
                        DBImage dbImage=new DBImage();
                        dbImage.image=ty;
                        dbImage.path=fileName;
                        images.Add(dbImage);
                        countImages++;
                    }
                }
                if (countImages == 0)
                {
                    pbtDelete.IsEnabled = false;
                    pbtNext.IsEnabled = false;
                    pbtPrevios.IsEnabled = false;
                    setNoImage();
                }
                else
                {
                    if (countImages == 1)
                    {
                        pbtNext.IsEnabled = false;
                        pbtPrevios.IsEnabled = false;
                    }
                    currImage = countImages - 1;
                    currentImage.Source = images[currImage].image;
                }
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show(ex.Message);
            }
        }

        private void pbtPrevios_Click(object sender, RoutedEventArgs e)
        {
            if (--currImage < 0)
                currImage = countImages-1;
            currentImage.Source = images[currImage].image;
        }

        private void pbtNext_Click(object sender, RoutedEventArgs e)
        {
            if (++currImage >=countImages)
                currImage = 0;
            currentImage.Source = images[currImage].image;
        }

        private void pbtDelete_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                string path = images[currImage].path;
                Process pro = Process.Start("..\\..\\..\\x64\\Debug\\MainColorMode.exe", "-delete \"" +
                    path + "\"");
                pro.WaitForExit();
                getImages();
                if (countImages == 0)
                {
                    pbtDelete.IsEnabled = false;
                    pbtNext.IsEnabled = false;
                    pbtPrevios.IsEnabled = false;
                    setNoImage();
                }
                else
                {
                    if (countImages == 1)
                    {
                        pbtNext.IsEnabled = false;
                        pbtPrevios.IsEnabled = false;
                    }
                }
                
                this.Title = "База данных изображений. Количество изображений: " + countImages.ToString();
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show(ex.Message);
            }
        }
        private void setNoImage()
        {
            currentImage.Source = noImage;
        }

        private void pbtAdd_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog addImageDialog = new OpenFileDialog();
            addImageDialog.FileName = "Изображение";
            addImageDialog.Filter = "Файлы изображений|*.jpg;*.bmp;*.png;*.jpeg";
            addImageDialog.DefaultExt = ".jpg";
            DialogResult result = new DialogResult();
            result=addImageDialog.ShowDialog();
            if (result == System.Windows.Forms.DialogResult.OK)
            {
                
                string imageFileName = addImageDialog.FileName;
                try
                {
                    addImage(imageFileName);
                    this.Title = "База данных изображений. Количество изображений: " + countImages.ToString();
                }
                catch (Exception ex)
                {
                    System.Windows.MessageBox.Show(ex.Message);
                }
            }
        }

        private void addImage(string imageFileName)
        {
            Process pro = Process.Start("..\\..\\..\\x64\\Debug\\MainColorMode.exe", "-add \"" + imageFileName + "\"");
            pro.WaitForExit();
            getImages();
            if (countImages == 1)
            {
                pbtDelete.IsEnabled = true;
            }
            else if (countImages == 2)
            {
                pbtNext.IsEnabled = true;
                pbtPrevios.IsEnabled = true;
            }
        }

        private void pbtAddCatalog_Click(object sender, RoutedEventArgs e)
        {
            string path = null;
            using (var dialog = new FolderBrowserDialog())
            {
                if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    path = dialog.SelectedPath;
                    foreach (string fileName in Directory.EnumerateFiles(path))
                    {
                        if (fileName.Contains(".png") || fileName.Contains(".jpg") ||
                            fileName.Contains(".jpeg") || fileName.Contains(".bmp"))
                        {
                            addImage(fileName);
                        }
                    }
                }
            }
        }
    }
}
