// @(#) $Id$


// Author: Anders Vestbo <mailto:vestbo@fi.uib.no>
//*-- Copyright &copy ALICE HLT Group

#include "AliL3StandardIncludes.h"

#include "AliL3Logging.h"
#include "AliL3MemHandler.h"
#include "AliL3Transform.h"
#include "AliL3DigitData.h"
#include "AliL3HistogramAdaptive.h"
#include "AliL3HoughTrack.h"
#include "AliL3HoughTransformerRow.h"

#if GCCVERSION == 3
using namespace std;
#endif

//_____________________________________________________________
// AliL3HoughTransformerRow
//
// TPC rows Hough transformation class
//

ClassImp(AliL3HoughTransformerRow)

Float_t AliL3HoughTransformerRow::fgBeta1 = 1.0/AliL3Transform::Row2X(79);
Float_t AliL3HoughTransformerRow::fgBeta2 = 1.0/(AliL3Transform::Row2X(158)*(1.0+tan(AliL3Transform::Pi()*10/180)*tan(AliL3Transform::Pi()*10/180)));

AliL3HoughTransformerRow::AliL3HoughTransformerRow()
{
  //Default constructor
  fParamSpace = 0;

  fGapCount = 0;
  fCurrentRowCount = 0;
#ifdef do_mc
  fTrackID = 0;
#endif
  fTrackNRows = 0;
  fTrackFirstRow = 0;
  fTrackLastRow = 0;
  fInitialGapCount = 0;

  fPrevBin = 0;
  fNextBin = 0;
  fNextRow = 0;

  fStartPadParams = 0;
  fEndPadParams = 0;
  fLUTr2 = 0;
  fLUTforwardZ = 0;
  fLUTforwardZ2 = 0;
  fLUTbackwardZ = 0;
  fLUTbackwardZ2 = 0;

}

AliL3HoughTransformerRow::AliL3HoughTransformerRow(Int_t slice,Int_t patch,Int_t netasegments,Bool_t /*DoMC*/,Float_t zvertex) : AliL3HoughBaseTransformer(slice,patch,netasegments,zvertex)
{
  //Normal constructor
  fParamSpace = 0;

  fGapCount = 0;
  fCurrentRowCount = 0;
#ifdef do_mc
  fTrackID = 0;
#endif

  fTrackNRows = 0;
  fTrackFirstRow = 0;
  fTrackLastRow = 0;
  fInitialGapCount = 0;

  fPrevBin = 0;
  fNextBin = 0;
  fNextRow = 0;

  fStartPadParams = 0;
  fEndPadParams = 0;
  fLUTr2 = 0;
  fLUTforwardZ = 0;
  fLUTforwardZ2 = 0;
  fLUTbackwardZ = 0;
  fLUTbackwardZ2 = 0;

}

AliL3HoughTransformerRow::~AliL3HoughTransformerRow()
{
  //Destructor
  DeleteHistograms();
  if(fLastTransformer) return;
#ifdef do_mc
  if(fTrackID)
    {
      for(Int_t i=0; i<GetNEtaSegments(); i++)
	{
	  if(!fTrackID[i]) continue;
	  delete fTrackID[i];
	}
      delete [] fTrackID;
      fTrackID = 0;
    }
#endif

  if(fGapCount)
    {
      for(Int_t i=0; i<GetNEtaSegments(); i++)
	{
	  if(!fGapCount[i]) continue;
	  delete [] fGapCount[i];
	}
      delete [] fGapCount;
      fGapCount = 0;
    }
  if(fCurrentRowCount)
    {
      for(Int_t i=0; i<GetNEtaSegments(); i++)
	{
	  if(fCurrentRowCount[i])
	    delete [] fCurrentRowCount[i];
	}
      delete [] fCurrentRowCount;
      fCurrentRowCount = 0;
    }
  if(fTrackNRows)
    {
      delete [] fTrackNRows;
      fTrackNRows = 0;
    }
  if(fTrackFirstRow)
    {
      delete [] fTrackFirstRow;
      fTrackFirstRow = 0;
    }
  if(fTrackLastRow)
    {
      delete [] fTrackLastRow;
      fTrackLastRow = 0;
    }
  if(fInitialGapCount)
    {
      delete [] fInitialGapCount;
      fInitialGapCount = 0;
    }
  if(fPrevBin)
    {
      for(Int_t i=0; i<GetNEtaSegments(); i++)
	{
	  if(!fPrevBin[i]) continue;
	  delete [] fPrevBin[i];
	}
      delete [] fPrevBin;
      fPrevBin = 0;
    }
  if(fNextBin)
    {
      for(Int_t i=0; i<GetNEtaSegments(); i++)
	{
	  if(!fNextBin[i]) continue;
	  delete [] fNextBin[i];
	}
      delete [] fNextBin;
      fNextBin = 0;
    }
  if(fNextRow)
    {
      for(Int_t i=0; i<GetNEtaSegments(); i++)
	{
	  if(!fNextRow[i]) continue;
	  delete [] fNextRow[i];
	}
      delete [] fNextRow;
      fNextRow = 0;
    }
  if(fStartPadParams)
    {
      for(Int_t i = AliL3Transform::GetFirstRow(0); i<=AliL3Transform::GetLastRow(5); i++)
	{
	  if(!fStartPadParams[i]) continue;
	  delete [] fStartPadParams[i];
	}
      delete [] fStartPadParams;
      fStartPadParams = 0;
    }
  if(fEndPadParams)
    {
      for(Int_t i = AliL3Transform::GetFirstRow(0); i<=AliL3Transform::GetLastRow(5); i++)
	{
	  if(!fEndPadParams[i]) continue;
	  delete [] fEndPadParams[i];
	}
      delete [] fEndPadParams;
      fEndPadParams = 0;
    }
  if(fLUTr2)
    {
      for(Int_t i = AliL3Transform::GetFirstRow(0); i<=AliL3Transform::GetLastRow(5); i++)
	{
	  if(!fLUTr2[i]) continue;
	  delete [] fLUTr2[i];
	}
      delete [] fLUTr2;
      fLUTr2 = 0;
    }
  if(fLUTforwardZ)
    {
      delete[] fLUTforwardZ;
      fLUTforwardZ=0;
    }
  if(fLUTforwardZ2)
    {
      delete[] fLUTforwardZ2;
      fLUTforwardZ2=0;
    }
  if(fLUTbackwardZ)
    {
      delete[] fLUTbackwardZ;
      fLUTbackwardZ=0;
    }
  if(fLUTbackwardZ2)
    {
      delete[] fLUTbackwardZ2;
      fLUTbackwardZ2=0;
    }
}

void AliL3HoughTransformerRow::DeleteHistograms()
{
  // Clean up
  if(!fParamSpace)
    return;
  for(Int_t i=0; i<GetNEtaSegments(); i++)
    {
      if(!fParamSpace[i]) continue;
      delete fParamSpace[i];
    }
  delete [] fParamSpace;
}

struct AliL3TrackLength {
  Bool_t fIsFilled;
  UInt_t fFirstRow;
  UInt_t fLastRow;
  Float_t fTrackPt;
};

void AliL3HoughTransformerRow::CreateHistograms(Int_t nxbin,Float_t xmin,Float_t xmax,
						Int_t nybin,Float_t ymin,Float_t ymax)
{
  //Create the histograms (parameter space)  
  //nxbin = #bins in X
  //nybin = #bins in Y
  //xmin xmax ymin ymax = histogram limits in X and Y
  fParamSpace = new AliL3Histogram*[GetNEtaSegments()];
  
  Char_t histname[256];
  for(Int_t i=0; i<GetNEtaSegments(); i++)
    {
      sprintf(histname,"paramspace_%d",i);
      fParamSpace[i] = new AliL3Histogram(histname,"",nxbin,xmin,xmax,nybin,ymin,ymax);
    }

  if(fLastTransformer) {

    fGapCount = ((AliL3HoughTransformerRow *)fLastTransformer)->fGapCount;
    fCurrentRowCount = ((AliL3HoughTransformerRow *)fLastTransformer)->fCurrentRowCount;
#ifdef do_mc
    fTrackID = ((AliL3HoughTransformerRow *)fLastTransformer)->fTrackID;
#endif
    fTrackNRows = ((AliL3HoughTransformerRow *)fLastTransformer)->fTrackNRows;
    fTrackFirstRow = ((AliL3HoughTransformerRow *)fLastTransformer)->fTrackFirstRow;
    fTrackLastRow = ((AliL3HoughTransformerRow *)fLastTransformer)->fTrackLastRow;
    fInitialGapCount = ((AliL3HoughTransformerRow *)fLastTransformer)->fInitialGapCount;

    fPrevBin = ((AliL3HoughTransformerRow *)fLastTransformer)->fPrevBin;
    fNextBin = ((AliL3HoughTransformerRow *)fLastTransformer)->fNextBin;
    fNextRow = ((AliL3HoughTransformerRow *)fLastTransformer)->fNextRow;

    fStartPadParams = ((AliL3HoughTransformerRow *)fLastTransformer)->fStartPadParams;
    fEndPadParams = ((AliL3HoughTransformerRow *)fLastTransformer)->fEndPadParams;
    fLUTr2 = ((AliL3HoughTransformerRow *)fLastTransformer)->fLUTr2;
    fLUTforwardZ = ((AliL3HoughTransformerRow *)fLastTransformer)->fLUTforwardZ;
    fLUTforwardZ2 = ((AliL3HoughTransformerRow *)fLastTransformer)->fLUTforwardZ2;
    fLUTbackwardZ = ((AliL3HoughTransformerRow *)fLastTransformer)->fLUTbackwardZ;
    fLUTbackwardZ2 = ((AliL3HoughTransformerRow *)fLastTransformer)->fLUTbackwardZ2;

    return;
  }
#ifdef do_mc
  {
    AliL3Histogram *hist = fParamSpace[0];
    Int_t ncellsx = (hist->GetNbinsX()+3)/2;
    Int_t ncellsy = (hist->GetNbinsY()+3)/2;
    Int_t ncells = ncellsx*ncellsy;
    if(!fTrackID)
      {
	LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	  <<"Transformer: Allocating "<<GetNEtaSegments()*ncells*sizeof(AliL3TrackIndex)<<" bytes to fTrackID"<<ENDLOG;
	fTrackID = new AliL3TrackIndex*[GetNEtaSegments()];
	for(Int_t i=0; i<GetNEtaSegments(); i++)
	  fTrackID[i] = new AliL3TrackIndex[ncells];
      }
  }
#endif
  AliL3Histogram *hist = fParamSpace[0];
  Int_t ncells = (hist->GetNbinsX()+2)*(hist->GetNbinsY()+2);
  if(!fGapCount)
    {
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating "<<GetNEtaSegments()*ncells*sizeof(UChar_t)<<" bytes to fGapCount"<<ENDLOG;
      fGapCount = new UChar_t*[GetNEtaSegments()];
      for(Int_t i=0; i<GetNEtaSegments(); i++)
	fGapCount[i] = new UChar_t[ncells];
    }
  if(!fCurrentRowCount)
    {
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating "<<GetNEtaSegments()*ncells*sizeof(UChar_t)<<" bytes to fCurrentRowCount"<<ENDLOG;
      fCurrentRowCount = new UChar_t*[GetNEtaSegments()];
      for(Int_t i=0; i<GetNEtaSegments(); i++)
	fCurrentRowCount[i] = new UChar_t[ncells];
    }
  if(!fPrevBin)
    {
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating "<<GetNEtaSegments()*ncells*sizeof(UChar_t)<<" bytes to fPrevBin"<<ENDLOG;
      fPrevBin = new UChar_t*[GetNEtaSegments()];
      for(Int_t i=0; i<GetNEtaSegments(); i++)
	fPrevBin[i] = new UChar_t[ncells];
    }
  if(!fNextBin)
    {
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating "<<GetNEtaSegments()*ncells*sizeof(UChar_t)<<" bytes to fNextBin"<<ENDLOG;
      fNextBin = new UChar_t*[GetNEtaSegments()];
      for(Int_t i=0; i<GetNEtaSegments(); i++)
	fNextBin[i] = new UChar_t[ncells];
    }
  Int_t ncellsy = hist->GetNbinsY()+2;
  if(!fNextRow)
    {
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating "<<GetNEtaSegments()*ncellsy*sizeof(UChar_t)<<" bytes to fNextRow"<<ENDLOG;
      fNextRow = new UChar_t*[GetNEtaSegments()];
      for(Int_t i=0; i<GetNEtaSegments(); i++)
	fNextRow[i] = new UChar_t[ncellsy];
    }

  if(!fTrackNRows)
    {
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating "<<ncells*sizeof(UChar_t)<<" bytes to fTrackNRows"<<ENDLOG;
      fTrackNRows = new UChar_t[ncells];
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating "<<ncells*sizeof(UChar_t)<<" bytes to fTrackFirstRow"<<ENDLOG;
      fTrackFirstRow = new UChar_t[ncells];
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating "<<ncells*sizeof(UChar_t)<<" bytes to fTrackLastRow"<<ENDLOG;
      fTrackLastRow = new UChar_t[ncells];
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating "<<ncells*sizeof(UChar_t)<<" bytes to fInitialGapCount"<<ENDLOG;
      fInitialGapCount = new UChar_t[ncells];


      AliL3HoughTrack track;
      Int_t xmin = hist->GetFirstXbin();
      Int_t xmax = hist->GetLastXbin();
      Int_t xmiddle = (hist->GetNbinsX()+1)/2;
      Int_t ymin = hist->GetFirstYbin();
      Int_t ymax = hist->GetLastYbin();
      Int_t nxbins = hist->GetNbinsX()+2;
      Int_t nxgrid = (hist->GetNbinsX()+3)/2+1;
      Int_t nygrid = hist->GetNbinsY()+3;

      AliL3TrackLength *tracklength = new AliL3TrackLength[nxgrid*nygrid];
      memset(tracklength,0,nxgrid*nygrid*sizeof(AliL3TrackLength));

      for(Int_t ybin=ymin-1; ybin<=(ymax+1); ybin++)
	{
	  for(Int_t xbin=xmin-1; xbin<=xmiddle; xbin++)
	    {
	      fTrackNRows[xbin + ybin*nxbins] = 255;
	      for(Int_t deltay = 0; deltay <= 1; deltay++) {
		for(Int_t deltax = 0; deltax <= 1; deltax++) {

		  AliL3TrackLength *curtracklength = &tracklength[(xbin + deltax) + (ybin + deltay)*nxgrid];
		  UInt_t maxfirstrow = 0;
		  UInt_t maxlastrow = 0;
		  Float_t maxtrackpt = 0;
		  if(curtracklength->fIsFilled) {
		    maxfirstrow = curtracklength->fFirstRow;
		    maxlastrow = curtracklength->fLastRow;
		    maxtrackpt = curtracklength->fTrackPt;
		  }
		  else {
		    Float_t xtrack = hist->GetPreciseBinCenterX((Float_t)xbin+0.5*(Float_t)(2*deltax-1));
		    Float_t ytrack = hist->GetPreciseBinCenterY((Float_t)ybin+0.5*(Float_t)(2*deltay-1));

		    Float_t psi = atan((xtrack-ytrack)/(fgBeta1-fgBeta2));
		    Float_t kappa = 2.0*(xtrack*cos(psi)-fgBeta1*sin(psi));
		    track.SetTrackParameters(kappa,psi,1);
		    Bool_t firstrow = kFALSE;
		    UInt_t curfirstrow = 0;
		    UInt_t curlastrow = 0;

		    Double_t centerx = track.GetCenterX();
		    Double_t centery = track.GetCenterY();
		    Double_t radius = track.GetRadius();

		    for(Int_t j=AliL3Transform::GetFirstRow(0); j<=AliL3Transform::GetLastRow(5); j++)
		      {
			Float_t hit[3];
			//		      if(!track.GetCrossingPoint(j,hit)) continue;
			hit[0] = AliL3Transform::Row2X(j);
			Double_t aa = (hit[0] - centerx)*(hit[0] - centerx);
			Double_t r2 = radius*radius;
			if(aa > r2)
			  continue;

			Double_t aa2 = sqrt(r2 - aa);
			Double_t y1 = centery + aa2;
			Double_t y2 = centery - aa2;
			hit[1] = y1;
			if(fabs(y2) < fabs(y1)) hit[1] = y2;
 
			hit[2] = 0;
			
			AliL3Transform::LocHLT2Raw(hit,0,j);
			hit[1] += 0.5;
			if(hit[1]>=0 && hit[1]<AliL3Transform::GetNPads(j))
			  {
			    if(!firstrow) {
			      curfirstrow = j;
			      firstrow = kTRUE;
			    }
			    curlastrow = j;
			  }
			else {
			  if(firstrow) {
			    firstrow = kFALSE;
			    if((curlastrow-curfirstrow) >= (maxlastrow-maxfirstrow)) {
			      maxfirstrow = curfirstrow;
			      maxlastrow = curlastrow;
			    }
			  }
			}
		      }
		    if((curlastrow-curfirstrow) >= (maxlastrow-maxfirstrow)) {
		      maxfirstrow = curfirstrow;
		      maxlastrow = curlastrow;
		    }

		    maxtrackpt = track.GetPt();

		    curtracklength->fIsFilled = kTRUE;
		    curtracklength->fFirstRow = maxfirstrow;
		    curtracklength->fLastRow = maxlastrow;
		    curtracklength->fTrackPt = maxtrackpt;
		    
		  }
		  if((maxlastrow-maxfirstrow) < fTrackNRows[xbin + ybin*nxbins]) {
		    fTrackNRows[xbin + ybin*nxbins] = maxlastrow-maxfirstrow;
		    fInitialGapCount[xbin + ybin*nxbins] = 1;
                    if((maxlastrow-maxfirstrow+1)<MIN_TRACK_LENGTH)
                      fInitialGapCount[xbin + ybin*nxbins] = MAX_N_GAPS+1;
		    if(maxtrackpt < 0.9*0.1*AliL3Transform::GetSolenoidField())
		      fInitialGapCount[xbin + ybin*nxbins] = MAX_N_GAPS;
		    fTrackFirstRow[xbin + ybin*nxbins] = maxfirstrow;
		    fTrackLastRow[xbin + ybin*nxbins] = maxlastrow;

		    Int_t xmirror = xmax - xbin + 1;
		    Int_t ymirror = ymax - ybin + 1;
		    fTrackNRows[xmirror + ymirror*nxbins] = fTrackNRows[xbin + ybin*nxbins];
		    fInitialGapCount[xmirror + ymirror*nxbins] = fInitialGapCount[xbin + ybin*nxbins];
		    fTrackFirstRow[xmirror + ymirror*nxbins] = fTrackFirstRow[xbin + ybin*nxbins];
		    fTrackLastRow[xmirror + ymirror*nxbins] = fTrackLastRow[xbin + ybin*nxbins];
		  }
		}
	      }
	      //	      cout<<" fTrackNRows "<<(Int_t)fInitialGapCount[xbin + ybin*nxbins]<<" "<<xbin<<" "<<ybin<<" "<<(Int_t)fTrackNRows[xbin + ybin*nxbins]<<" "<<(Int_t)fTrackFirstRow[xbin + ybin*nxbins]<<" "<<(Int_t)fTrackLastRow[xbin + ybin*nxbins]<<" "<<endl;
	    }
	}
      delete [] tracklength;
    }

  if(!fStartPadParams)
    {
      Int_t nrows = AliL3Transform::GetLastRow(5) - AliL3Transform::GetFirstRow(0) + 1;
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating about "<<nrows*100*sizeof(AliL3PadHoughParams)<<" bytes to fStartPadParams"<<ENDLOG;
      fStartPadParams = new AliL3PadHoughParams*[nrows];
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating about "<<nrows*100*sizeof(AliL3PadHoughParams)<<" bytes to fEndPadParams"<<ENDLOG;
      fEndPadParams = new AliL3PadHoughParams*[nrows];
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating about "<<nrows*100*sizeof(Float_t)<<" bytes to fLUTr2"<<ENDLOG;
      fLUTr2 = new Float_t*[nrows];

      Float_t beta1 = fgBeta1;
      Float_t beta2 = fgBeta2;
      Float_t beta1minusbeta2 = fgBeta1 - fgBeta2;
      Float_t ymin = hist->GetYmin();
      Float_t histbin = hist->GetBinWidthY();
      Float_t xmin = hist->GetXmin();
      Float_t xmax = hist->GetXmax();
      Float_t xbin = (xmax-xmin)/hist->GetNbinsX();
      Int_t firstbinx = hist->GetFirstXbin();
      Int_t lastbinx = hist->GetLastXbin();
      Int_t nbinx = hist->GetNbinsX()+2;
      Int_t firstbin = hist->GetFirstYbin();
      Int_t lastbin = hist->GetLastYbin();
      for(Int_t i=AliL3Transform::GetFirstRow(0); i<=AliL3Transform::GetLastRow(5); i++)
	{
	  Int_t npads = AliL3Transform::GetNPads(i);
	  Int_t ipatch = AliL3Transform::GetPatch(i);
	  Double_t padpitch = AliL3Transform::GetPadPitchWidth(ipatch);
	  Float_t x = AliL3Transform::Row2X(i);
	  Float_t x2 = x*x;

	  fStartPadParams[i] = new AliL3PadHoughParams[npads];
	  fEndPadParams[i] = new AliL3PadHoughParams[npads];
	  fLUTr2[i] = new Float_t[npads];
	  for(Int_t pad=0; pad<npads; pad++)
	    {
	      Float_t y = (pad-0.5*(npads-1))*padpitch;
	      fLUTr2[i][pad] = x2 + y*y;
	      Float_t starty = (pad-0.5*npads)*padpitch;
	      Float_t r1 = x2 + starty*starty;
	      Float_t xoverr1 = x/r1;
	      Float_t startyoverr1 = starty/r1;
	      Float_t endy = (pad-0.5*(npads-2))*padpitch;
	      Float_t r2 = x2 + endy*endy; 
	      Float_t xoverr2 = x/r2;
	      Float_t endyoverr2 = endy/r2;
	      Float_t a1 = beta1minusbeta2/(xoverr1-beta2);
	      Float_t b1 = (xoverr1-beta1)/(xoverr1-beta2);
	      Float_t a2 = beta1minusbeta2/(xoverr2-beta2);
	      Float_t b2 = (xoverr2-beta1)/(xoverr2-beta2);

	      Float_t alpha1 = (a1*startyoverr1+b1*ymin-xmin)/xbin;
	      Float_t deltaalpha1 = b1*histbin/xbin;
	      if(b1<0)
		alpha1 += deltaalpha1;
	      Float_t alpha2 = (a2*endyoverr2+b2*ymin-xmin)/xbin;
	      Float_t deltaalpha2 = b2*histbin/xbin;
	      if(b2>=0)
		alpha2 += deltaalpha2;

	      fStartPadParams[i][pad].fAlpha = alpha1;
	      fStartPadParams[i][pad].fDeltaAlpha = deltaalpha1;
	      fEndPadParams[i][pad].fAlpha = alpha2;
	      fEndPadParams[i][pad].fDeltaAlpha = deltaalpha2;

	      //Find the first and last bin rows to be filled
	      Bool_t binfound1 = kFALSE;
	      Bool_t binfound2 = kFALSE;
	      Int_t firstbin1 = lastbin;
	      Int_t firstbin2 = lastbin;
	      Int_t lastbin1 = firstbin;
	      Int_t lastbin2 = firstbin;
	      for(Int_t b=firstbin; b<=lastbin; b++, alpha1 += deltaalpha1, alpha2 += deltaalpha2)
		{
		  Int_t binx1 = 1 + (Int_t)alpha1;
		  if(binx1<=lastbinx) {
		    UChar_t initialgapcount; 
		    if(binx1>=firstbinx)
		      initialgapcount = fInitialGapCount[binx1 + b*nbinx];
		    else
		      initialgapcount = fInitialGapCount[firstbinx + b*nbinx];
		    if(initialgapcount != MAX_N_GAPS) {
		      if(!binfound1) {
			firstbin1 = b;
			binfound1 = kTRUE;
		      }
		      lastbin1 = b;
		    }
		  }
		  Int_t binx2 = 1 + (Int_t)alpha2;
		  if(binx2>=firstbin) {
		    UChar_t initialgapcount; 
		    if(binx2<=lastbinx)
		      initialgapcount = fInitialGapCount[binx2 + b*nbinx];
		    else
		      initialgapcount = fInitialGapCount[lastbinx + b*nbinx];
		    if(initialgapcount != MAX_N_GAPS) {
		      if(!binfound2) {
			firstbin2 = b;
			binfound2 = kTRUE;
		      }
		      lastbin2 = b;
		    }
		  }
		}
	      fStartPadParams[i][pad].fFirstBin=firstbin1;
	      fStartPadParams[i][pad].fLastBin=lastbin1;
	      fEndPadParams[i][pad].fFirstBin=firstbin2;
	      fEndPadParams[i][pad].fLastBin=lastbin2;
	    }
	}
    }
 
  //create lookup table for z of the digits
  if(!fLUTforwardZ)
    {
      Int_t ntimebins = AliL3Transform::GetNTimeBins();
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating "<<ntimebins*sizeof(Float_t)<<" bytes to fLUTforwardZ"<<ENDLOG;
      fLUTforwardZ = new Float_t[ntimebins];
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating "<<ntimebins*sizeof(Float_t)<<" bytes to fLUTforwardZ2"<<ENDLOG;
      fLUTforwardZ2 = new Float_t[ntimebins];
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating "<<ntimebins*sizeof(Float_t)<<" bytes to fLUTbackwardZ"<<ENDLOG;
      fLUTbackwardZ = new Float_t[ntimebins];
      LOG(AliL3Log::kInformational,"AliL3HoughTransformerRow::CreateHistograms()","")
	<<"Transformer: Allocating "<<ntimebins*sizeof(Float_t)<<" bytes to fLUTbackwardZ2"<<ENDLOG;
      fLUTbackwardZ2 = new Float_t[ntimebins];
      for(Int_t i=0; i<ntimebins; i++){
	Float_t z;
	z=AliL3Transform::GetZFast(0,i,GetZVertex());
	fLUTforwardZ[i]=z;
	fLUTforwardZ2[i]=z*z;
	z=AliL3Transform::GetZFast(18,i,GetZVertex());
	fLUTbackwardZ[i]=z;
	fLUTbackwardZ2[i]=z*z;
      }
    }
}

void AliL3HoughTransformerRow::Reset()
{
  //Reset all the histograms. Should be done when processing new slice

  if(!fParamSpace)
    {
      LOG(AliL3Log::kWarning,"AliL3HoughTransformer::Reset","Histograms")
	<<"No histograms to reset"<<ENDLOG;
      return;
    }
  
  for(Int_t i=0; i<GetNEtaSegments(); i++)
    fParamSpace[i]->Reset();
  
#ifdef do_mc
  {
    AliL3Histogram *hist = fParamSpace[0];
    Int_t ncellsx = (hist->GetNbinsX()+3)/2;
    Int_t ncellsy = (hist->GetNbinsY()+3)/2;
    Int_t ncells = ncellsx*ncellsy;
    for(Int_t i=0; i<GetNEtaSegments(); i++)
      memset(fTrackID[i],0,ncells*sizeof(AliL3TrackIndex));
  }
#endif
  AliL3Histogram *hist = fParamSpace[0];
  Int_t ncells = (hist->GetNbinsX()+2)*(hist->GetNbinsY()+2);
  for(Int_t i=0; i<GetNEtaSegments(); i++)
    {
      memcpy(fGapCount[i],fInitialGapCount,ncells*sizeof(UChar_t));
      memcpy(fCurrentRowCount[i],fTrackFirstRow,ncells*sizeof(UChar_t));
    }
}

Int_t AliL3HoughTransformerRow::GetEtaIndex(Double_t eta) const
{
  //Return the histogram index of the corresponding eta. 

  Double_t etaslice = (GetEtaMax() - GetEtaMin())/GetNEtaSegments();
  Double_t index = (eta-GetEtaMin())/etaslice;
  return (Int_t)index;
}

inline AliL3Histogram *AliL3HoughTransformerRow::GetHistogram(Int_t etaindex)
{
  // Return a pointer to the histogram which contains etaindex eta slice
  if(!fParamSpace || etaindex >= GetNEtaSegments() || etaindex < 0)
    return 0;
  if(!fParamSpace[etaindex])
    return 0;
  return fParamSpace[etaindex];
}

Double_t AliL3HoughTransformerRow::GetEta(Int_t etaindex,Int_t /*slice*/) const
{
  // Return eta calculated in the middle of the eta slice
  Double_t etaslice = (GetEtaMax()-GetEtaMin())/GetNEtaSegments();
  Double_t eta=0;
  eta=(Double_t)((etaindex+0.5)*etaslice);
  return eta;
}

void AliL3HoughTransformerRow::TransformCircle()
{
  if(GetDataPointer())
    TransformCircleFromDigitArray();
  else if(fTPCRawStream)
    TransformCircleFromRawStream();
}
void AliL3HoughTransformerRow::TransformCircleFromDigitArray()
{
  //Do the Hough Transform

  Int_t netasegments = GetNEtaSegments();
  Double_t etamin = GetEtaMin();
  Double_t etaslice = (GetEtaMax() - etamin)/netasegments;

  Int_t lowerthreshold = GetLowerThreshold();

  //Assumes that all the etaslice histos are the same!
  AliL3Histogram *h = fParamSpace[0];
  Int_t firstbiny = h->GetFirstYbin();
  Int_t firstbinx = h->GetFirstXbin();
  Int_t lastbinx = h->GetLastXbin();
  Int_t nbinx = h->GetNbinsX()+2;

  UChar_t lastpad;
  Int_t lastetaindex;
  AliL3EtaRow *etaclust = new AliL3EtaRow[netasegments];

  AliL3DigitRowData *tempPt = GetDataPointer();
  if(!tempPt)
    {
      LOG(AliL3Log::kError,"AliL3HoughTransformer::TransformCircle","Data")
	<<"No input data "<<ENDLOG;
      return;
    }

  Int_t ipatch = GetPatch();
  Int_t ilastpatch = GetLastPatch();
  Int_t islice = GetSlice();
  Float_t *lutz;
  Float_t *lutz2;
  if(islice < 18) {
    lutz = fLUTforwardZ;
    lutz2 = fLUTforwardZ2;
  }
  else {
    lutz = fLUTbackwardZ;
    lutz2 = fLUTbackwardZ2;
  }

  //Loop over the padrows:
  for(UChar_t i=AliL3Transform::GetFirstRow(ipatch); i<=AliL3Transform::GetLastRow(ipatch); i++)
    {
      lastpad = 255;
      //Flush eta clusters array
      memset(etaclust,0,netasegments*sizeof(AliL3EtaRow));  

      Float_t x = AliL3Transform::Row2X((Int_t)i);
      Float_t x2 = x*x;
      Float_t radius2=0;

      //Get the data on this padrow:
      AliL3DigitData *digPt = tempPt->fDigitData;
      if((Int_t)i != (Int_t)tempPt->fRow)
	{
	  LOG(AliL3Log::kError,"AliL3HoughTransformerRow::TransformCircle","Data")
	    <<"AliL3HoughTransform::TransformCircle : Mismatching padrow numbering "<<(Int_t)i<<" "<<(Int_t)tempPt->fRow<<ENDLOG;
	  continue;
	}
      //      cout<<" Starting row "<<i<<endl;
      //Loop over the data on this padrow:
      for(UInt_t j=0; j<tempPt->fNDigit; j++)
	{
	  UShort_t charge = digPt[j].fCharge;
	  if((Int_t)charge <= lowerthreshold)
	    continue;
	  UChar_t pad = digPt[j].fPad;
	  UShort_t time = digPt[j].fTime;

	  if(pad != lastpad)
	    {
	      radius2 = fLUTr2[(Int_t)i][(Int_t)pad];
	      lastetaindex = -1;
	    }

	  //Transform data to local cartesian coordinates:
	  Float_t z = lutz[(Int_t)time];
	  Float_t z2 = lutz2[(Int_t)time];
	  // Acceptance cut : to be verified
	  //	  if(radius2<0.72406166*z2) continue;
	  if(x2<0.8464*z2) continue;
	  //Calculate the eta:
	  Double_t r = sqrt(radius2+z2);
	  Double_t eta = 0.5 * log((r+z)/(r-z));
	  //Get the corresponding index, which determines which histogram to fill:
	  Int_t etaindex = (Int_t)((eta-etamin)/etaslice);

#ifndef do_mc
	  if(etaindex == lastetaindex) continue;
#endif
	  //	  cout<<" Digit at patch "<<ipatch<<" row "<<i<<" pad "<<(Int_t)pad<<" time "<<time<<" etaslice "<<etaindex<<endl;
	  
	  if(etaindex < 0 || etaindex >= netasegments)
	    continue;

	  if(!etaclust[etaindex].fIsFound)
	    {
	      etaclust[etaindex].fStartPad = pad;
	      etaclust[etaindex].fEndPad = pad;
	      etaclust[etaindex].fIsFound = 1;
#ifdef do_mc
	      FillClusterMCLabels(digPt[j],&etaclust[etaindex]);
#endif
	      continue;
	    }
	  else
	    {
	      if(pad <= (etaclust[etaindex].fEndPad + 1))
		{
		  etaclust[etaindex].fEndPad = pad;
#ifdef do_mc
		  FillClusterMCLabels(digPt[j],&etaclust[etaindex]);
#endif
		}
	      else
		{
		  FillCluster(i,etaindex,etaclust,ilastpatch,firstbinx,lastbinx,nbinx,firstbiny);

		  etaclust[etaindex].fStartPad = pad;
		  etaclust[etaindex].fEndPad = pad;

#ifdef do_mc
		  memset(etaclust[etaindex].fMcLabels,0,MaxTrack);
		  FillClusterMCLabels(digPt[j],&etaclust[etaindex]);
#endif
		}
	    }
	  lastpad = pad;
	  lastetaindex = etaindex;
	}
      //Write remaining clusters
      for(Int_t etaindex = 0;etaindex < netasegments;etaindex++)
	{
	  //Check for empty row
	  if((etaclust[etaindex].fStartPad == 0) && (etaclust[etaindex].fEndPad == 0)) continue;

	  FillCluster(i,etaindex,etaclust,ilastpatch,firstbinx,lastbinx,nbinx,firstbiny);
	}

      //Move the data pointer to the next padrow:
      AliL3MemHandler::UpdateRowPointer(tempPt);
    }

  delete [] etaclust;
}

void AliL3HoughTransformerRow::TransformCircleFromRawStream()
{
  //Do the Hough Transform

  Int_t netasegments = GetNEtaSegments();
  Double_t etamin = GetEtaMin();
  Double_t etaslice = (GetEtaMax() - etamin)/netasegments;

  Int_t lowerthreshold = GetLowerThreshold();

  //Assumes that all the etaslice histos are the same!
  AliL3Histogram *h = fParamSpace[0];
  Int_t firstbiny = h->GetFirstYbin();
  Int_t firstbinx = h->GetFirstXbin();
  Int_t lastbinx = h->GetLastXbin();
  Int_t nbinx = h->GetNbinsX()+2;

  Int_t lastetaindex;
  AliL3EtaRow *etaclust = new AliL3EtaRow[netasegments];

  if(!fTPCRawStream)
    {
      LOG(AliL3Log::kError,"AliL3HoughTransformer::TransformCircle","Data")
	<<"No input data "<<ENDLOG;
      return;
    }

  Int_t ipatch = GetPatch();
  UChar_t rowmin = AliL3Transform::GetFirstRowOnDDL(ipatch);
  UChar_t rowmax = AliL3Transform::GetLastRowOnDDL(ipatch);
  //  Int_t ntimebins = AliL3Transform::GetNTimeBins();
  Int_t ilastpatch = GetLastPatch();
  Int_t islice = GetSlice();
  Float_t *lutz;
  Float_t *lutz2;
  if(islice < 18) {
    lutz = fLUTforwardZ;
    lutz2 = fLUTforwardZ2;
  }
  else {
    lutz = fLUTbackwardZ;
    lutz2 = fLUTbackwardZ2;
  }

  //Flush eta clusters array
  memset(etaclust,0,netasegments*sizeof(AliL3EtaRow));  

  UChar_t i=0;
  Int_t npads=0;
  Float_t x=0;
  Float_t x2=0;
  Float_t radius2=0;
  UChar_t pad=0;

  //Loop over the rawdata:
  while (fTPCRawStream->Next()) {
    
    if(fTPCRawStream->IsNewSector() || fTPCRawStream->IsNewRow()) {

      //Write remaining clusters
      for(Int_t etaindex = 0;etaindex < netasegments;etaindex++)
	{
	  //Check for empty row
	  if((etaclust[etaindex].fStartPad == 0) && (etaclust[etaindex].fEndPad == 0)) continue;
      
	  FillCluster(i,etaindex,etaclust,ilastpatch,firstbinx,lastbinx,nbinx,firstbiny);
	}

      Int_t sector=fTPCRawStream->GetSector();
      Int_t row=fTPCRawStream->GetRow();
      Int_t slice,srow;
      AliL3Transform::Sector2Slice(slice,srow,sector,row);
      if(slice!=islice){
	LOG(AliL3Log::kError,"AliL3DDLDataFileHandler::DDLDigits2Memory","Slice")
	  <<AliL3Log::kDec<<"Found slice "<<slice<<", expected "<<islice<<ENDLOG;
	continue;
      }
    
      i=(UChar_t)srow;
      npads = AliL3Transform::GetNPads(srow)-1;

      //Flush eta clusters array
      memset(etaclust,0,netasegments*sizeof(AliL3EtaRow));  

      x = AliL3Transform::Row2X((Int_t)i);
      x2 = x*x;
      radius2=0;

    }

    if((i<rowmin)||(i>rowmax))continue;

    //    cout<<" Starting row "<<(UInt_t)i<<endl;
    //Loop over the data on this padrow:
    if(fTPCRawStream->IsNewRow() || fTPCRawStream->IsNewPad()) {
      pad=fTPCRawStream->GetPad();
      /*
      if((pad<0)||(pad>=(npads+1))){
	LOG(AliL3Log::kError,"AliL3DDLDataFileHandler::DDLDigits2Memory","Pad")
	  <<AliL3Log::kDec<<"Pad value out of bounds "<<pad<<" "
	  <<npads+1<<ENDLOG;
	continue;
      }
      */
      radius2 = fLUTr2[(Int_t)i][(Int_t)pad];
      lastetaindex = -1;
    } 

    UShort_t time=fTPCRawStream->GetTime();
    /*
    if((time<0)||(time>=ntimebins)){
      LOG(AliL3Log::kError,"AliL3DDLDataFileHandler::DDLDigits2Memory","Time")
	<<AliL3Log::kDec<<"Time out of bounds "<<time<<" "
	<<AliL3Transform::GetNTimeBins()<<ENDLOG;
      continue;
    }
    */

    if(fTPCRawStream->GetSignal() <= lowerthreshold)
      continue;

    //Transform data to local cartesian coordinates:
    Float_t z = lutz[(Int_t)time];
    Float_t z2 = lutz2[(Int_t)time];
    //    if(radius2<0.72406166*z2) continue;
    if(x2<0.8464*z2) continue;
    //Calculate the eta:
    Double_t r = sqrt(radius2+z2);
    Double_t eta = 0.5 * log((r+z)/(r-z));
    //Get the corresponding index, which determines which histogram to fill:
    Int_t etaindex = (Int_t)((eta-etamin)/etaslice);

#ifndef do_mc
    if(etaindex == lastetaindex) continue;
#endif
    //	  cout<<" Digit at patch "<<ipatch<<" row "<<i<<" pad "<<(Int_t)pad<<" time "<<time<<" etaslice "<<etaindex<<endl;
	  
    if(etaindex < 0 || etaindex >= netasegments)
      continue;
    
    if(!etaclust[etaindex].fIsFound)
      {
	etaclust[etaindex].fStartPad = pad;
	etaclust[etaindex].fEndPad = pad;
	etaclust[etaindex].fIsFound = 1;
	continue;
      }
    else
      {
	if(pad <= (etaclust[etaindex].fEndPad + 1))
	  {
	    etaclust[etaindex].fEndPad = pad;
	  }
	else
	  {
	    FillCluster(i,etaindex,etaclust,ilastpatch,firstbinx,lastbinx,nbinx,firstbiny);
	    
	    etaclust[etaindex].fStartPad = pad;
	    etaclust[etaindex].fEndPad = pad;
	    
	  }
      }
    lastetaindex = etaindex;
  }

  //Write remaining clusters
  for(Int_t etaindex = 0;etaindex < netasegments;etaindex++)
    {
      //Check for empty row
      if((etaclust[etaindex].fStartPad == 0) && (etaclust[etaindex].fEndPad == 0)) continue;
      
      FillCluster(i,etaindex,etaclust,ilastpatch,firstbinx,lastbinx,nbinx,firstbiny);
    }
  
  delete [] etaclust;
}

Int_t AliL3HoughTransformerRow::GetTrackID(Int_t etaindex,Double_t alpha1,Double_t alpha2) const
{
  // Returns the MC label for a given peak found in the Hough space
#ifndef do_mc
  LOG(AliL3Log::kWarning,"AliL3HoughTransformerRow::GetTrackID","Data")
    <<"Flag switched off"<<ENDLOG;
  return -1;
#else
  if(etaindex < 0 || etaindex > GetNEtaSegments())
    {
      LOG(AliL3Log::kWarning,"AliL3HoughTransformerRow::GetTrackID","Data")
	<<"Wrong etaindex "<<etaindex<<ENDLOG;
      return -1;
    }
  AliL3Histogram *hist = fParamSpace[etaindex];
  Int_t bin = hist->FindLabelBin(alpha1,alpha2);
  if(bin==-1) {
    LOG(AliL3Log::kWarning,"AliL3HoughTransformerRow::GetTrackID()","")
      <<"Track candidate outside Hough space boundaries: "<<alpha1<<" "<<alpha2<<ENDLOG;
    return -1;
  }
  Int_t label=-1;
  Int_t max=0;
  for(UInt_t i=0; i<(MaxTrack-1); i++)
    {
      Int_t nhits=fTrackID[etaindex][bin].fNHits[i];
      if(nhits == 0) break;
      if(nhits > max)
	{
	  max = nhits;
	  label = fTrackID[etaindex][bin].fLabel[i];
	}
    }
  Int_t label2=-1;
  Int_t max2=0;
  for(UInt_t i=0; i<(MaxTrack-1); i++)
    {
      Int_t nhits=fTrackID[etaindex][bin].fNHits[i];
      if(nhits == 0) break;
      if(nhits > max2)
	{
	  if(fTrackID[etaindex][bin].fLabel[i]!=label) {
	    max2 = nhits;
	    label2 = fTrackID[etaindex][bin].fLabel[i];
	  }
	}
    }
  if(max2 !=0 ) {
    LOG(AliL3Log::kDebug,"AliL3HoughTransformerRow::GetTrackID()","")
      <<" TrackID"<<" label "<<label<<" max "<<max<<" label2 "<<label2<<" max2 "<<max2<<" "<<(Float_t)max2/(Float_t)max<<" "<<fTrackID[etaindex][bin].fLabel[MaxTrack-1]<<" "<<(Int_t)fTrackID[etaindex][bin].fNHits[MaxTrack-1]<<ENDLOG;
  }
  return label;
#endif
  LOG(AliL3Log::kWarning,"AliL3HoughTransformerRow::GetTrackID()","")
    <<"Compile with do_mc flag!"<<ENDLOG;
  return -1;
}

UChar_t *AliL3HoughTransformerRow::GetGapCount(Int_t etaindex)
{
  return fGapCount[etaindex];
}

UChar_t *AliL3HoughTransformerRow::GetCurrentRowCount(Int_t etaindex)
{
  return fCurrentRowCount[etaindex];
}

UChar_t *AliL3HoughTransformerRow::GetTrackNRows()
{
  return fTrackNRows;
}


UChar_t *AliL3HoughTransformerRow::GetTrackFirstRow()
{
  return fTrackFirstRow;
}

UChar_t *AliL3HoughTransformerRow::GetTrackLastRow()
{
  return fTrackLastRow;
}

UChar_t *AliL3HoughTransformerRow::GetPrevBin(Int_t etaindex)
{
  return fPrevBin[etaindex];
}

UChar_t *AliL3HoughTransformerRow::GetNextBin(Int_t etaindex)
{
  return fNextBin[etaindex];
}

UChar_t *AliL3HoughTransformerRow::GetNextRow(Int_t etaindex)
{
  return fNextRow[etaindex];
}

inline void AliL3HoughTransformerRow::FillClusterRow(UChar_t i,Int_t binx1,Int_t binx2,UChar_t *ngaps2,UChar_t *currentrow2,UChar_t *lastrow2
#ifdef do_mc
			   ,AliL3EtaRow etaclust,AliL3TrackIndex *trackid
#endif
			   )
{
  for(Int_t bin=binx1;bin<=binx2;bin++)
    {
      if(ngaps2[bin] < MAX_N_GAPS) {
	if(i < lastrow2[bin] && i > currentrow2[bin]) {
	  ngaps2[bin] += (i-currentrow2[bin]-1);
	  currentrow2[bin]=i;
	}
#ifdef do_mc
	if(i < lastrow2[bin] && i >= currentrow2[bin]) {
	  for(UInt_t t=0;t<(MaxTrack-1); t++)
	    {
	      Int_t label = etaclust.fMcLabels[t];
	      if(label == 0) break;
	      UInt_t c;
	      Int_t tempbin2 = (Int_t)(bin/2);
	      for(c=0; c<(MaxTrack-1); c++)
		if(trackid[tempbin2].fLabel[c] == label || trackid[tempbin2].fNHits[c] == 0)
		  break;
	      trackid[tempbin2].fLabel[c] = label;
	      if(trackid[tempbin2].fCurrentRow[c] != i) {
		trackid[tempbin2].fNHits[c]++;
		trackid[tempbin2].fCurrentRow[c] = i;
	      }
	    }
	}
#endif
      }
    }

}

inline void AliL3HoughTransformerRow::FillCluster(UChar_t i,Int_t etaindex,AliL3EtaRow *etaclust,Int_t ilastpatch,Int_t firstbinx,Int_t lastbinx,Int_t nbinx,Int_t firstbiny)
{
  UChar_t *ngaps = fGapCount[etaindex];
  UChar_t *currentrow = fCurrentRowCount[etaindex];
  UChar_t *lastrow = fTrackLastRow;
  UChar_t *prevbin = fPrevBin[etaindex];
  UChar_t *nextbin = fNextBin[etaindex];
  UChar_t *nextrow = fNextRow[etaindex];
#ifdef do_mc
  AliL3TrackIndex *trackid = fTrackID[etaindex];
#endif

  //Do the transformation:
  AliL3PadHoughParams *startparams = &fStartPadParams[(Int_t)i][etaclust[etaindex].fStartPad]; 
  AliL3PadHoughParams *endparams = &fEndPadParams[(Int_t)i][etaclust[etaindex].fEndPad];
 
  Float_t alpha1 = startparams->fAlpha;
  Float_t deltaalpha1 = startparams->fDeltaAlpha;
  Float_t alpha2 = endparams->fAlpha;
  Float_t deltaalpha2 = endparams->fDeltaAlpha;

  Int_t firstbin1 = startparams->fFirstBin;
  Int_t firstbin2 = endparams->fFirstBin;
  Int_t firstbin = firstbin1;
  if(firstbin>firstbin2) firstbin = firstbin2;

  Int_t lastbin1 = startparams->fLastBin;
  Int_t lastbin2 = endparams->fLastBin;
  Int_t lastbin = lastbin1;
  if(lastbin<lastbin2) lastbin = lastbin2;
  
  alpha1 += (firstbin-firstbiny)*deltaalpha1;
  alpha2 += (firstbin-firstbiny)*deltaalpha2;

  //Fill the histogram along the alpha2 range
  if(ilastpatch == -1) {
    for(Int_t b=firstbin; b<=lastbin; b++, alpha1 += deltaalpha1, alpha2 += deltaalpha2)
      {
	Int_t binx1 = 1 + (Int_t)alpha1;
	if(binx1>lastbinx) continue;
	if(binx1<firstbinx) binx1 = firstbinx;
	Int_t binx2 = 1 + (Int_t)alpha2;
	if(binx2<firstbinx) continue;
	if(binx2>lastbinx) binx2 = lastbinx;
#ifdef do_mc
	if(binx2<binx1) {
	  LOG(AliL3Log::kWarning,"AliL3HoughTransformerRow::TransformCircle()","")
	    <<"Wrong filling "<<binx1<<" "<<binx2<<" "<<i<<" "<<etaclust[etaindex].fStartPad<<" "<<etaclust[etaindex].fEndPad<<ENDLOG;
	}
#endif
	Int_t tempbin = b*nbinx;
	UChar_t *ngaps2 = ngaps + tempbin;
	UChar_t *currentrow2 = currentrow + tempbin;
	UChar_t *lastrow2 = lastrow + tempbin;
#ifdef do_mc
	Int_t tempbin2 = ((Int_t)(b/2))*((Int_t)((nbinx+1)/2));
	AliL3TrackIndex *trackid2 = trackid + tempbin2;
#endif
	FillClusterRow(i,binx1,binx2,ngaps2,currentrow2,lastrow2
#ifdef do_mc
		       ,etaclust[etaindex],trackid2
#endif
		       );
      }
  }
  else {
    for(Int_t b=firstbin; b<=lastbin; b++, alpha1 += deltaalpha1, alpha2 += deltaalpha2)
      {
	Int_t binx1 = 1 + (Int_t)alpha1;
	if(binx1>lastbinx) continue;
	if(binx1<firstbinx) binx1 = firstbinx;
	Int_t binx2 = 1 + (Int_t)alpha2;
	if(binx2<firstbinx) continue;
	if(binx2>lastbinx) binx2 = lastbinx;
#ifdef do_mc
	if(binx2<binx1) {
	  LOG(AliL3Log::kWarning,"AliL3HoughTransformerRow::TransformCircle()","")
	    <<"Wrong filling "<<binx1<<" "<<binx2<<" "<<i<<" "<<etaclust[etaindex].fStartPad<<" "<<etaclust[etaindex].fEndPad<<ENDLOG;
	}
#endif
	if(nextrow[b] > (b + 1)) {
	  Int_t deltab = (nextrow[b] - b - 1);
	  b += deltab;
	  alpha1 += deltaalpha1*deltab;
	  alpha2 += deltaalpha2*deltab;
	  continue;
	}
	Int_t tempbin = b*nbinx;
	binx1 = (UInt_t)nextbin[binx1 + tempbin];
	binx2 = (UInt_t)prevbin[binx2 + tempbin];
	if(binx2<binx1) continue;
	UChar_t *ngaps2 = ngaps + tempbin;
	UChar_t *currentrow2 = currentrow + tempbin;
	UChar_t *lastrow2 = lastrow + tempbin;
#ifdef do_mc
	Int_t tempbin2 = ((Int_t)(b/2))*((Int_t)((nbinx+1)/2));
	AliL3TrackIndex *trackid2 = trackid + tempbin2;
#endif
	FillClusterRow(i,binx1,binx2,ngaps2,currentrow2,lastrow2
#ifdef do_mc
		       ,etaclust[etaindex],trackid2
#endif
		       );
      }
  }
}

#ifdef do_mc
inline void AliL3HoughTransformerRow::FillClusterMCLabels(AliL3DigitData digpt,AliL3EtaRow *etaclust)
{
  for(Int_t t=0; t<3; t++)
    {
      Int_t label = digpt.fTrackID[t];
      if(label < 0) break;
      UInt_t c;
      for(c=0; c<(MaxTrack-1); c++)
	if(etaclust->fMcLabels[c] == label || etaclust->fMcLabels[c] == 0)
	  break;

      etaclust->fMcLabels[c] = label;
    }
}
#endif
