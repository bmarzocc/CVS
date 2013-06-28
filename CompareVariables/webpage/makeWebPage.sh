#!/bin/bash

usage='Usage: -d <inputDir> -s <name1> -r <name2> -f <comptype> -l <htmldir> -w <whichdir>'



args=`getopt rd: -- "$@"`
if test $? != 0
     then
         echo $usage
         exit 1
fi



eval set -- "$args"
for i
  do
  case "$i" in
      -d) shift; input_dir=$2;shift;;
      -s) shift; name1=$2;shift;;
      -r) shift; name2=$2;shift;;
      -f) shift; comptype=$2;shift;;
      -l) shift; htmldir=$2;shift;;
      -w) shift; whichdir=$2;shift;;
  esac
done

if [ "X"${input_dir} == "X" ]
    then
    echo
    echo "MISSING INPUT FILE NAME! Please give a valid one!"
    echo $usage
    exit
fi

if [ "X"${name1} == "X" ]
    then
    echo
    echo "MISSING FIRST NAME! Please give a valid one!"
    echo $usage
    exit
fi

if [ "X"${name2} == "X" ]
    then
    echo
    echo "MISSING SECOND NAME! Please give a valid one!"
    echo $usage
    exit
fi

if [ "X"${comptype} == "X" ]
    then
    comptype=DA_vs_MC
    echo
    echo "using default comptype:" ${comptype}
fi


if [ "X"${htmldir} == "X" ]
    then
    htmldir=/afs/cern.ch/user/c/ccecal/scratch0/www/ValidationPlots/MC_studies
    echo
    echo "using default htmldir:" ${htmldir}
fi


if [ "X"${whichdir} == "X" ]
    then
    whichdir=Data
    echo
    echo "using default httpdir: Data" 
fi

echo
echo 'Preparing Validation Webpages' 


# specify directories here
cd ../../../
my_cmssw_base=`\pwd`
work_dir=${my_cmssw_base}/ECALSimulation/CompareVariables
echo
echo 'WorkDir: ' $work_dir

echo 
echo 'OutputDir: '${whichdir}

plots_dir=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_ECAL/ccecal/ValidationPlots/MC_studies/${whichdir};

http_dir=http://test-ecal-cosmics.web.cern.ch/test-ecal-cosmics/ValidationPlots/MC_studies/${whichdir};

mkdir ${plots_dir}

cp ${input_dir}/* ${plots_dir}

echo
echo 'Making webpage for '${name1}' vs '${name2}''

cat > ${plots_dir}/webpage.html << EOF

<HTML>

<HEAD><TITLE> ECAL VALIDATION PLOTS ${name1} vs ${name2} </TITLE></HEAD>
 
<BODY link="color: rgb(0, 0, 253);">
<FONT color="Black">

<Center>
<h1> ECAL Validation </h1>
</Center>

<hr>

<Center>
<h3>  <FONT color="Black"> ${name1}  <FONT color="Blue"> vs <FONT color="Red"> ${name2}  </h3>
</center>


<FONT color="Black">

<h4> Validation Plots </h4>
<ul>
 <li> Simulation Control-plots
 <ul>
 <li><A href="#PVN"> Number of PV </A><BR>
 <li><A href="#InvariantMass"> Invariant Mass </A><BR>
 <li><A href="#LaserCorrVsEta"> LaserCorr vs Eta</A><BR>
 <li><A href="#NoiseVsEta"> Noise vs Eta </A><BR>
</ul>
 <li> Electrons Control-plots
 <ul>
 <li><A href="#rho"> Rho </A><BR> 
 <li><A href="#emIso"> emIso </A><BR>
 <li><A href="#pfIso"> pfIso </A><BR>
 <li><A href="#scEta"> scEta </A><BR>
 <li><A href="#scPhi"> scEta </A><BR>
 <li><A href="#DetaIn"> DetaIn </A><BR>
 <li><A href="#DphiIn"> DphiIn </A><BR>
 <li><A href="#scEtaWidth"> scEtaWidth </A><BR>
 <li><A href="#scPhiWidth"> scPhiWidth </A><BR>
 <li><A href="#sigmaIetaIeta"> sigmaIetaIeta </A><BR>
 <li><A href="#sigmaPhi/sigmaEta"> sigmaPhi/sigmaEta </A><BR>
 <li><A href="#scEt"> scEt </A><BR>
 <li><A href="#R9"> R9  </A><BR>
 <li><A href="#scCorrection_vs_scEta"> scCorrection_vs_scEta  </A><BR>
</ul>

<hr>


<h3><A name="PVN"> Number of PV  </h3>

<A HREF=${http_dir}/comp_h_PV_n_${comptype}.png> <img height="350" src="${http_dir}/comp_h_PV_n_${comptype}.png"> </A>

<hr>

<h3><A name="InvariantMass"> Invariant Mass  </h3>

<A HREF=${http_dir}/comp_h_ele1ele2_scM_total_${comptype}.png> <img height="350" src="${http_dir}/comp_h_ele1ele2_scM_total_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_ele1ele2_scM_EBEB_${comptype}.png> <img height="350" src="${http_dir}/comp_h_ele1ele2_scM_EBEB_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_ele1ele2_scM_EEEE_${comptype}.png> <img height="350" src="${http_dir}/comp_h_ele1ele2_scM_EEEE_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_ele1ele2_scM_EBEE_${comptype}.png> <img height="350" src="${http_dir}/comp_h_ele1ele2_scM_EBEE_${comptype}.png"> </A>

<hr>


<h3><A name="LaserCorrVsEta"> LaserCorr vs Eta  </h3>

<A HREF=${http_dir}/comp_h_LC_vs_scEta_${comptype}.png> <img height="350" src="${http_dir}/comp_h_LC_vs_scEta_${comptype}.png"> </A>

<hr>


<h3><A name="NoiseVsEta"> Noise vs Eta  </h3>

<A HREF=${http_dir}/h_Noise_vs_scEta.png> <img height="350" src="${http_dir}/h_Noise_vs_scEta.png"> </A>

<hr>


<h3><A name="rho"> Rho  </h3>

<A HREF=${http_dir}/comp_h_rho_total_${comptype}.png> <img height="350" src="${http_dir}/comp_h_rho_total_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_rho_EBEB_${comptype}.png> <img height="350" src="${http_dir}/comp_h_rho_EBEB_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_rho_EEEE_${comptype}.png> <img height="350" src="${http_dir}/comp_h_rho_EEEE_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_rho_EBEE_${comptype}.png> <img height="350" src="${http_dir}/comp_h_rho_EBEE_${comptype}.png"> </A>

<hr>


<h3><A name="emIso"> emIso  </h3>

<A HREF=${http_dir}/comp_h_emIso_total_EB_${comptype}.png> <img height="350" src="${http_dir}/comp_h_emIso_total_EB_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_emIso_total_EE_${comptype}.png> <img height="350" src="${http_dir}/comp_h_emIso_total_EE_${comptype}.png"> </A>

<hr>


<h3><A name="emIso"> pfIso  </h3>

<A HREF=${http_dir}/comp_h_pfIso_total_EB_${comptype}.png> <img height="350" src="${http_dir}/comp_h_pfIso_total_EB_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_pfIso_total_EE_${comptype}.png> <img height="350" src="${http_dir}/comp_h_pfIso_total_EE_${comptype}.png"> </A>

<hr>


<h3><A name="scEta"> scEta  </h3>

<A HREF=${http_dir}/comp_h_scEta_total_EB_${comptype}.png> <img height="350" src="${http_dir}/comp_h_scEta_total_EB_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_scEta_total_EE_${comptype}.png> <img height="350" src="${http_dir}/comp_h_scEta_total_EE_${comptype}.png"> </A>

<hr>


<h3><A name="scPhi"> scPhi  </h3>

<A HREF=${http_dir}/comp_h_scPhi_total_EB_${comptype}.png> <img height="350" src="${http_dir}/comp_h_scPhi_total_EB_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_scPhi_total_EE_DA_vs_MC.png> <img height="350" src="${http_dir}/comp_h_scPhi_total_EE_${comptype}.png"> </A>

<hr>


<h3><A name="DetaIn"> DetaIn  </h3>

<A HREF=${http_dir}/comp_h_DetaIn_total_EB_${comptype}.png> <img height="350" src="${http_dir}/comp_h_DetaIn_total_EB_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_DetaIn_total_EE_${comptype}.png> <img height="350" src="${http_dir}/comp_h_DetaIn_total_EE_${comptype}.png"> </A>

<hr>


<h3><A name="DphiIn"> DphiIn  </h3>

<A HREF=${http_dir}/comp_h_DphiIn_total_EB_${comptype}.png> <img height="350" src="${http_dir}/comp_h_DphiIn_total_EB_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_DphiIn_total_EE_${comptype}.png> <img height="350" src="${http_dir}/comp_h_DphiIn_total_EE_${comptype}.png"> </A>

<hr>


<h3><A name="scEtaWidth"> scEtaWidth  </h3>

<A HREF=${http_dir}/comp_h_scEtaWidth_total_EB_${comptype}.png> <img height="350" src="${http_dir}/comp_h_scEtaWidth_total_EB_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_scEtaWidth_total_EE_${comptype}.png> <img height="350" src="${http_dir}/comp_h_scEtaWidth_total_EE_${comptype}.png"> </A>

<hr>


<h3><A name="scPhiWidth"> scPhiWidth  </h3>

<A HREF=${http_dir}/comp_h_scPhiWidth_total_EB_${comptype}.png> <img height="350" src="${http_dir}/comp_h_scPhiWidth_total_EB_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_scPhiWidth_total_EE_${comptype}.png> <img height="350" src="${http_dir}/comp_h_scPhiWidth_total_EE_${comptype}.png"> </A>

<hr>


<h3><A name="sigmaIetaIeta"> sigmaIetaIeta  </h3>

<A HREF=${http_dir}/comp_h_sigmaIetaIeta_total_EB_${comptype}.png> <img height="350" src="${http_dir}/comp_h_sigmaIetaIeta_total_EB_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_sigmaIetaIeta_EE_${comptype}.png> <img height="350" src="${http_dir}/comp_h_sigmaIetaIeta_total_EE_${comptype}.png"> </A>

<hr>


<h3><A name="sigmaPhi/sigmaEta"> sigmaPhi/sigmaEta  </h3>

<A HREF=${http_dir}/comp_h_sigmaIetaIeta_total_EB_${comptype}.png> <img height="350" src="${http_dir}/comp_h_sigmaPhi_sigmaEta_total_EB_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_sigmaIetaIeta_EE_${comptype}.png> <img height="350" src="${http_dir}/comp_h_sigmaPhi_sigmaEta_total_EE_${comptype}.png"> </A>

<hr>


<h3><A name="scEt"> scEt  </h3>

<A HREF=${http_dir}/comp_h_scEt_total_EB_${comptype}.png> <img height="350" src="${http_dir}/comp_h_scEt_total_EB_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_scEt_EE_${comptype}png> <img height="350" src="${http_dir}/comp_h_scEt_total_EE_${comptype}.png"> </A>

<hr>


<h3><A name="R9"> R9  </h3>

<A HREF=${http_dir}/comp_h_R9_EB_${comptype}.png> <img height="350" src="${http_dir}/comp_h_R9_EB_${comptype}.png"> </A>

<A HREF=${http_dir}/comp_h_R9_EE_${comptype}.png> <img height="350" src="${http_dir}/comp_h_R9_EE_${comptype}.png"> </A>

<hr>


<h3><A name="scCorrection_vs_scEta"> scCorrection_vs_scEta  </h3>

<A HREF=${http_dir}/comp_h_scCorrection_vs_scEta_${comptype}.png> <img height="350" src="${http_dir}/comp_h_scCorrection_vs_scEta_${comptype}.png"> </A>

<hr>



EOF
echo
echo 'ECAL validation results are available here:'
echo  http://test-ecal-cosmics.web.cern.ch/test-ecal-cosmics/ValidationPlots/MC_studies/${whichdir}/webpage.html
echo


