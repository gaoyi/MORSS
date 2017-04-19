#include <iostream>


#include "include/SFLSMultiRobustStatSegmentor3D.h"


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


template<typename pixel_t> 
itk::Image<unsigned char, 3>::Pointer sfls2label(typename itk::Image<pixel_t, 3>::Pointer sfls, unsigned char label = 1);


int main(int argc, char** argv)
{
  if (argc != 7)
    {
      std::cerr<<"Parameters: inputImage labelImageName outputImagePrefix numIter intensityHomo[0~1] lambda[0~1]\n";
      exit(-1);
    }

  const char* inputImage = argv[1];
  const char* labelImageName = argv[2];
  const char* outputImagePrefix = argv[3];

  unsigned long numiter = atol(argv[4]);
  double intensityHomo = atof(argv[5]);
  double lambda = atof(argv[6]);


  typedef int PixelType;
  typedef SFLSMultiRobustStatSegmentor3D_c< PixelType > SFLSMultiRobustStatSegmentor3D_t;

  // read input image
  //typedef SFLSMultiRobustStatSegmentor3D_t::TDoubleImage Image_t;
  typedef itk::Image<PixelType, 3> Image_t;

  typedef itk::ImageFileReader< Image_t > ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName(inputImage);
  Image_t::Pointer img;
    
  try
    {
      reader->Update();
      img = reader->GetOutput();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cerr<< "ExceptionObject caught !" << std::endl; 
      std::cerr<< err << std::endl; 
      raise(SIGABRT);
    }



  // read input label image
  typedef SFLSMultiRobustStatSegmentor3D_t::labelMap_t labelMap_t;

  typedef itk::ImageFileReader< labelMap_t > labelMapReader_t;
  labelMapReader_t::Pointer readerLabel = labelMapReader_t::New();
  readerLabel->SetFileName(labelImageName);
  labelMap_t::Pointer labelImg;
    
  try
    {
      readerLabel->Update();
      labelImg = readerLabel->GetOutput();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cerr<< "ExceptionObject caught !" << std::endl; 
      std::cerr<< err << std::endl; 
      raise(SIGABRT);
    }


  // do seg
  SFLSMultiRobustStatSegmentor3D_t seg;
  seg.setImage(img);

  seg.setLabelMap(labelImg);

  seg.setNumIter(numiter);

  seg.setIntensityHomogeneity(intensityHomo);
  seg.setCurvatureWeight(lambda);

  seg.doSegmenation();

  for (short iObj = 0; iObj < seg.getNumberOfObjects(); ++iObj)
    {
      char thisOutputImage[1000];
      sprintf(thisOutputImage, "%s_%d_%ld_%1.3f_%1.3f.nrrd", outputImagePrefix, iObj, numiter, intensityHomo, lambda);


      //typedef itk::ImageFileWriter< SFLSMultiRobustStatSegmentor3D_t::LSImageType > WriterType;
      typedef itk::ImageFileWriter< itk::Image<unsigned char, 3> > WriterType;

      WriterType::Pointer writer = WriterType::New();
      writer->SetFileName( thisOutputImage );
      //writer->SetInput(seg.m_phiList[iObj]);
      writer->SetInput(sfls2label<float>(seg.m_phiList[iObj], iObj+1));

      try
        {
          writer->Update();
        }
      catch ( itk::ExceptionObject &err)
        {
          std::cout << "ExceptionObject caught !" << std::endl; 
          std::cout << err << std::endl; 
          raise(SIGABRT);   
        }

    }

  
  return 0;
}




template<typename pixel_t> 
itk::Image<unsigned char, 3>::Pointer sfls2label(typename itk::Image<pixel_t, 3>::Pointer sfls, unsigned char label)
{
  typedef itk::Image<pixel_t, 3> itkImage_t;
  typedef itk::Image<unsigned char, 3> itkUcharImage_t;

  itkUcharImage_t::Pointer bin = itkUcharImage_t::New();
  bin->SetRegions(sfls->GetLargestPossibleRegion() );
  bin->CopyInformation(sfls);
  bin->Allocate();
  bin->FillBuffer(0);

  typedef itk::ImageRegionIterator< itkImage_t > itkImageRegionIterator_t;
  typedef itk::ImageRegionIterator< itkUcharImage_t > maskImageRegionIterator_t;

  itkImageRegionIterator_t it1( sfls, sfls->GetLargestPossibleRegion() );
  it1.GoToBegin();

  maskImageRegionIterator_t it2( bin, bin->GetLargestPossibleRegion() );
  it2.GoToBegin();
  
  for (; !it1.IsAtEnd(); ++it1, ++it2)
    {
      pixel_t v = it1.Get();

      if (v <= 0)
        {
          it2.Set(label);
        }
    }

  return bin;  
}
