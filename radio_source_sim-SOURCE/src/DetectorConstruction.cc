#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"    
#include "G4Torus.hh"   
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4IonisParamMat.hh"
#include "G4SubtractionSolid.hh"
#include "DetectorConstruction.hh"

DetectorConstruction::DetectorConstruction(const G4String& outputPrefix)
    : fOutputPrefix(outputPrefix)
{
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    G4NistManager* nist = G4NistManager::Instance();
    G4bool checkOverlaps = true;
    G4VPhysicalVolume* physAirRectoA[16] = { nullptr };
    G4VPhysicalVolume* physAirRectoB[16] = { nullptr };

    // -----------------------------------------------------
    // 1. MATERIALES 
    // -----------------------------------------------------
    G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* al_mat = nist->FindOrBuildMaterial("G4_Al");
    G4MaterialPropertiesTable* mptAir = new G4MaterialPropertiesTable();
    G4double airRIndex[] = { 1.0, 1.0 };
    G4double airEnergy[] = { 1.0 * eV, 5.0 * eV };
    mptAir->AddProperty("RINDEX", airEnergy, airRIndex, 2);
    world_mat->SetMaterialPropertiesTable(mptAir);
    //G4Material* scintMat = nist->FindOrBuildMaterial("G4_POLYSTYRENE");
    G4Material* scintMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    
    // -----------------------------------------------------
    // 1. PROPIEDADES ÓPTICAS
    // -----------------------------------------------------
    G4MaterialPropertiesTable* mptScint = new G4MaterialPropertiesTable();

    // Definimos un rango de energía (desde el UV al Visible)
    G4double photonEnergy[] = {
         1.800 * eV, 1.835 * eV, 1.891 * eV, 1.932 * eV, 1.968 * eV, 2.024 * eV, 2.079 * eV, 2.140 * eV,
         2.215 * eV, 2.296 * eV, 2.395 * eV, 2.491 * eV, 2.551 * eV, 2.662 * eV, 2.706 * eV, 2.749 * eV,
         2.784 * eV, 2.814 * eV, 2.846 * eV, 2.872 * eV, 2.900 * eV, 2.919 * eV, 2.932 * eV, 2.946 * eV,
         2.969 * eV, 2.987 * eV, 3.012 * eV, 3.025 * eV, 3.048 * eV, 3.098 * eV, 3.125 * eV, 3.159 * eV,
         3.204 * eV, 3.242 * eV, 3.275 * eV, 3.306 * eV, 3.351 * eV, 3.361 * eV, 3.396 * eV, 3.414 * eV,
         3.434 * eV, 3.453 * eV, 3.472 * eV, 3.511 * eV, 3.540 * eV, 3.591 * eV, 3.616 * eV, 3.656 * eV,
         3.696 * eV, 3.742 * eV, 3.796 * eV, 3.829 * eV, 3.863 * eV, 3.946 * eV, 4.037 * eV, 4.128 * eV
    };

    G4double emissionIntens[] = {
        0.24260, 0.23471, 0.23669, 0.24260, 0.25838, 0.26627, 0.27022, 0.30178,
        0.33136, 0.34320, 0.32333, 0.32150, 0.32939, 0.35306, 0.39842, 0.43393,
        0.48718, 0.60552, 0.69822, 0.74359, 0.78107, 0.81460, 0.86193, 0.92110,
        0.96252, 0.97633, 0.99408, 0.99211, 0.98225, 0.94477, 0.90533, 0.85010,
        0.79882, 0.77318, 0.70217, 0.62525, 0.55819, 0.54241, 0.53254, 0.52663,
        0.54043, 0.56016, 0.60750, 0.62130, 0.62919, 0.62327, 0.60552, 0.57594,
        0.51282, 0.47929, 0.44576, 0.36884, 0.31361, 0.28994, 0.27613, 0.26430
    };

    G4double energyAbs[] = {
        2.066 * eV, 2.147 * eV, 2.317 * eV, 2.395 * eV, 2.500 * eV, 2.556 * eV, 2.605 * eV, 2.661 * eV,
        2.722 * eV, 2.774 * eV, 2.811 * eV, 2.859 * eV, 2.896 * eV, 2.926 * eV, 2.955 * eV, 2.983 * eV,
        3.012 * eV, 3.028 * eV, 3.042 * eV, 3.056 * eV, 3.069 * eV, 3.083 * eV, 3.100 * eV, 3.189 * eV,
        3.375 * eV, 3.641 * eV, 4.133 * eV 
    };


    G4double absLengthScint[] = {
        14.05 * cm, 12.73 * cm, 10.51 * cm, 9.18 * cm,  8.55 * cm,  7.99 * cm,  7.71 * cm,  7.44 * cm,
        7.08 * cm,  6.69 * cm,  6.33 * cm,  5.62 * cm,  5.12 * cm,  4.77 * cm,  4.12 * cm,  3.36 * cm,
        2.44 * cm,  1.82 * cm,  1.36 * cm,  0.96 * cm,  0.66 * cm,  0.54 * cm,  0.52 * cm,  0.51 * cm,
        0.51 * cm,  0.51 * cm,  0.50 * cm
    };


    const G4int nAbs = sizeof(energyAbs) / sizeof(G4double);
    const G4int nEntries = sizeof(photonEnergy) / sizeof(G4double);

    std::vector<G4double> rIndex(nEntries, 1.58);

    mptScint->AddProperty("RINDEX", photonEnergy, rIndex.data(), nEntries);
    mptScint->AddProperty("ABSLENGTH", energyAbs, absLengthScint, nAbs);
    mptScint->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, emissionIntens, nEntries);

    // Parámetros de centelleo
    mptScint->AddConstProperty("SCINTILLATIONYIELD", 10000. / MeV);
    mptScint->AddConstProperty("RESOLUTIONSCALE", 1.0);
    mptScint->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1 * ns);
    mptScint->AddConstProperty("SCINTILLATIONYIELD1", 1.0);
    scintMat->SetMaterialPropertiesTable(mptScint);

    // -----------------------------------------------------
    // MATERIALES WLS
    // -----------------------------------------------------
    G4Material* coreMat = nist->FindOrBuildMaterial("G4_POLYSTYRENE");
    G4MaterialPropertiesTable* mptWLS = new G4MaterialPropertiesTable();

    // ====================================================================
    // ÓPTICA DE LA FIBRA WLS
    // ====================================================================

    G4double wlsEnergy[] = {
        1.7712 * eV,
        2.0000 * eV, 2.0248 * eV, 2.0690 * eV, 2.1015 * eV, 2.1388 * eV, 2.1722 * eV,
        2.2059 * eV, 2.2351 * eV, 2.2578 * eV, 2.2958 * eV, 2.3141 * eV, 2.3400 * eV,
        2.3764 * eV, 2.3916 * eV, 2.4253 * eV, 2.4431 * eV, 2.4562 * eV, 2.4724 * eV,
        2.4854 * eV, 2.4932 * eV, 2.5162 * eV, 2.5297 * eV, 2.5447 * eV, 2.5645 * eV,
        2.5819 * eV, 2.6011 * eV, 2.6200 * eV, 2.6397 * eV, 2.6560 * eV, 2.6718 * eV,
        2.6896 * eV,
        2.7552 * eV, 3.1000 * eV, 3.5410 * eV, 3.8500 * eV, 4.1328 * eV
    };
    const G4int nWls = sizeof(wlsEnergy) / sizeof(G4double);

    G4double wlsEmit[] = {
        0.00,
        0.00, 0.01, 0.04, 0.05, 0.06, 0.08, 0.13, 0.18, 0.23, 0.36,
        0.42, 0.45, 0.47, 0.53, 0.67, 0.77, 0.87, 0.97, 1.00, 1.00,
        0.89, 0.77, 0.66, 0.56, 0.46, 0.37, 0.24, 0.16, 0.10, 0.04,
        0.00,
        0.00, 0.00, 0.00, 0.00, 0.00
    };

    G4double wlsBulkAbs[] = {
        5.0 * m,
        5.0 * m,   5.0 * m,   5.0 * m,   5.0 * m,   4.8 * m,   4.5 * m,   4.2 * m,   4.0 * m,   3.8 * m,   3.5 * m,
        3.0 * m,   2.5 * m,   2.0 * m,   1.5 * m,   0.9 * m,   0.6 * m,   0.4 * m,   0.3 * m,   0.25 * m,  0.22 * m,
        0.15 * m,  12.0 * cm, 8.0 * cm,  5.0 * cm,  3.5 * cm,  2.0 * cm,  1.0 * cm,  0.5 * cm,  0.2 * cm,  0.1 * mm,
        0.1 * mm,
        0.1 * mm,  0.1 * mm,  0.1 * mm,  0.1 * mm,  0.1 * mm
    };

    G4double wlsRIndex[nWls];
    G4double wlsAbs[nWls];
    G4double rIndexInner[nWls];
    G4double rIndexOuter[nWls];

    for (G4int i = 0; i < nWls; i++) {
        wlsRIndex[i] = 1.59;
        rIndexInner[i] = 1.49;
        rIndexOuter[i] = 1.42;

        if (wlsEnergy[i] >= 2.65 * eV) {
            wlsAbs[i] = 0.04 * mm;
        }
        else {
            wlsAbs[i] = 10.0 * m;
        }
    }

    mptWLS->AddProperty("WLSCOMPONENT", wlsEnergy, wlsEmit, nWls);
    mptWLS->AddProperty("ABSLENGTH", wlsEnergy, wlsBulkAbs, nWls);
    mptWLS->AddProperty("RINDEX", wlsEnergy, wlsRIndex, nWls);
    mptWLS->AddProperty("WLSABSLENGTH", wlsEnergy, wlsAbs, nWls);
    mptWLS->AddConstProperty("WLSTIMECONSTANT", 7.2 * ns);

    coreMat->SetMaterialPropertiesTable(mptWLS);

    // --- MATERIALES PARA MULTI CLADDING ---
    G4Material* pmma = nist->FindOrBuildMaterial("G4_PLEXIGLASS"); // Inner Cladding (n=1.49)
    G4Element* elF = nist->FindOrBuildElement("F");
    G4Element* elC = nist->FindOrBuildElement("C");
    G4Material* fp = new G4Material("FluorinatedPolymer", 1.43 * g / cm3, 2); // Outer Cladding (n=1.42)
    fp->AddElement(elC, 2); fp->AddElement(elF, 4); 

    // Propiedades Ópticas
    G4MaterialPropertiesTable* mptInner = new G4MaterialPropertiesTable();
    mptInner->AddProperty("RINDEX", wlsEnergy, rIndexInner, nWls);
    pmma->SetMaterialPropertiesTable(mptInner);

    G4MaterialPropertiesTable* mptOuter = new G4MaterialPropertiesTable();
    mptOuter->AddProperty("RINDEX", wlsEnergy, rIndexOuter, nWls);
    fp->SetMaterialPropertiesTable(mptOuter);

    // -----------------------------------------------------
    // 2. SUPERFICIE REFLECTANTE (TiO2)
    // -----------------------------------------------------

    G4OpticalSurface* surfTiO2 = new G4OpticalSurface("TiO2Surface");
    surfTiO2->SetType(dielectric_metal);
    surfTiO2->SetFinish(ground); // Reflexión difusa 
    surfTiO2->SetModel(unified);

    G4MaterialPropertiesTable* mptSurf = new G4MaterialPropertiesTable();
    G4double reflectivity[] = { 0.95, 0.95 }; // 95% de reflexión
    mptSurf->AddProperty("REFLECTIVITY", photonEnergy, reflectivity, 2);
    surfTiO2->SetMaterialPropertiesTable(mptSurf);

    // Dióxido de Titanio (TiO2)
    G4Element* elTi = nist->FindOrBuildElement("Ti");
    G4Element* elO = nist->FindOrBuildElement("O");
    G4Material* TiO2 = new G4Material("TiO2", 4.23 * g / cm3, 2);
    TiO2->AddElement(elTi, 1);
    TiO2->AddElement(elO, 2);

    // -----------------------------------------------------
    // 2. DIMENSIONES
    // -----------------------------------------------------
    G4double inch = 2.54 * cm;
    G4double al_x = 36.0 * inch;
    G4double al_y = 90.0 * inch;
    G4double al_z = 1.5 * inch;
    G4double al_wall_thick = 0.050 * inch;

    G4double bar_x = 5.0 * cm;
    G4double bar_y = 200.0 * cm;
    G4double bar_z = 1.0 * cm;
    G4double paint_thick = 0.25 * mm;

    // 2. GEOMETRÍA DE LA FIBRA 
    G4double D = 1.0 * mm;
    G4double coreDiam = 0.88 * mm; 
    G4double innerDiam = 0.94 * mm;
    G4double fiber_extension = 7.0 * cm;

    // --- NUEVO: DIMENSIONES DE LA CAJA DEL DETECTOR ---
    G4double detector_x = al_x - 1 * cm;      
    G4double detector_y = 4.0 * cm;   
    G4double detector_z = al_z - 1 * cm;;      

    G4double half_fiber_length = 0.5 * bar_y + 0.5 * fiber_extension;
    G4Tubs* sOuterClad = new G4Tubs("OuterClad_S", 0, D / 2.,half_fiber_length, 0, 360 * deg);
    G4LogicalVolume* lOuterClad = new G4LogicalVolume(sOuterClad, fp, "OuterClad_L");

    G4Tubs* sInnerClad = new G4Tubs("InnerClad_S", 0, innerDiam / 2.,  half_fiber_length, 0, 360 * deg);
    G4LogicalVolume* lInnerClad = new G4LogicalVolume(sInnerClad, pmma, "InnerClad_L");

    G4Tubs* sCore = new G4Tubs("Core_S", 0, coreDiam / 2.,half_fiber_length, 0, 360 * deg);
    G4LogicalVolume* lCore = new G4LogicalVolume(sCore, coreMat, "Core_L");


    // -----------------------------------------------------
    // 3. CONSTRUCCIÓN
    // -----------------------------------------------------

    // WORLD
    auto solidWorld = new G4Box("World", 1.5 * al_x, 1.2 * al_y, 2.0 * al_z);
    auto logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");
    auto physWorld = new G4PVPlacement(nullptr, G4ThreeVector(), logicWorld, "World", nullptr, false, 0, checkOverlaps);

    // CAJA DE ALUMINIO
    auto solidAlBox = new G4Box("AlBox_Solid", 0.5 * al_x, 0.5 * al_y, 0.5 * al_z);
    auto logicAlBox = new G4LogicalVolume(solidAlBox, al_mat, "AlBox_Logic");
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logicAlBox, "AlBox_Phys", logicWorld, false, 0, checkOverlaps);

    // AIRE INTERNO 
    G4double inner_x = al_x - 2 * al_wall_thick;
    G4double inner_y = al_y - 2 * al_wall_thick;
    G4double inner_z = al_z - 2 * al_wall_thick;
    auto solidInner = new G4Box("Inner_Solid", 0.5 * inner_x, 0.5 * inner_y, 0.5 * inner_z);
    auto logicInner = new G4LogicalVolume(solidInner, world_mat, "Inner_Logic");
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logicInner, "Inner_Phys", logicAlBox, false, 0, checkOverlaps);

    // PINTURA TiO2 (Wrap) - MARCO HUECO 
    G4double wrap_x = bar_x + 2 * paint_thick;
    G4double wrap_y = bar_y; // Mismo largo que la barra para liberar las tapas
    G4double wrap_z = bar_z + 2 * paint_thick;

    auto solidWrapOuter = new G4Box("Wrap_Outer_Solid", 0.5 * wrap_x, 0.5 * wrap_y, 0.5 * wrap_z);

    auto solidWrapInner = new G4Box("Wrap_Inner_Solid", 0.5 * bar_x, 0.5 * wrap_y + 1.0 * mm, 0.5 * bar_z);

    auto solidWrap = new G4SubtractionSolid("Wrap_Solid", solidWrapOuter, solidWrapInner);

    auto logicWrap = new G4LogicalVolume(solidWrap, TiO2, "Wrap_Logic");

    auto solidScint = new G4Box("Scint_Solid", 0.5 * bar_x, 0.5 * bar_y, 0.5 * bar_z);

    // --- PERFORACIÓN ---
    G4double channelDiam = 2.5 * mm;
    G4double fiberDiam = 1.0 * mm;
    G4double xHole = 1.2 * cm; // Distancia entre el Agujero A y el Agujero B

    // --- 1. LOS CANALES DE AIRE  ---
    G4Tubs* solidAirChannel = new G4Tubs("AirChannel_S", 0, channelDiam / 2.,half_fiber_length, 0, 360 * deg);
    G4LogicalVolume* logicAirChannelA = new G4LogicalVolume(solidAirChannel, world_mat, "AirChannelA_L");
    G4LogicalVolume* logicAirChannelB = new G4LogicalVolume(solidAirChannel, world_mat, "AirChannelB_L");

    // --- 2. ENSAMBLAJE DE LA FIBRA ---
    new G4PVPlacement(nullptr, G4ThreeVector(), lCore, "Core_P", lInnerClad, false, 0, checkOverlaps);
    new G4PVPlacement(nullptr, G4ThreeVector(), lInnerClad, "InnerClad_P", lOuterClad, false, 0, checkOverlaps);

    new G4PVPlacement(nullptr, G4ThreeVector(), lOuterClad, "FiberA_InsideAir_P", logicAirChannelA, false, 0, checkOverlaps);
    new G4PVPlacement(nullptr, G4ThreeVector(), lOuterClad, "FiberB_InsideAir_P", logicAirChannelB, false, 0, checkOverlaps);
    
    G4RotationMatrix* rotTube = new G4RotationMatrix();
    rotTube->rotateX(90 * deg);
    G4LogicalVolume* scoringScintLogic = nullptr;

    // -----------------------------------------------------
    // COLOCACIÓN DE LAS 16 BARRAS 
    // -----------------------------------------------------
    for (G4int i = 0; i < 16; i++) {
        G4double xPos = -40.0 * cm + (i * wrap_x) + (0.5 * wrap_x);

        G4String wrapName = "Wrap_Logic_" + std::to_string(i);
        G4LogicalVolume* localLogicWrap = new G4LogicalVolume(solidWrap, TiO2, wrapName);

        G4String scintName = "Scint_Logic_" + std::to_string(i);
        G4LogicalVolume* localLogicScint = new G4LogicalVolume(solidScint, scintMat, scintName);
        localLogicScint->GetMaterial()->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);

        if (i == 0) { fScoringVolume = localLogicScint; }

        G4String scintPhysName = "Scint_Phys_" + std::to_string(i);
        G4VPhysicalVolume* scintPhys = new G4PVPlacement(nullptr,
            G4ThreeVector(xPos, 0, 0),
            localLogicScint,
            scintPhysName,
            logicInner,
            false,
            i,
            checkOverlaps);

        G4String barPhysName = "Bar_Phys_" + std::to_string(i);
        G4VPhysicalVolume* barPhys = new G4PVPlacement(
            nullptr, G4ThreeVector(xPos, 0, 0),
            localLogicWrap,
            barPhysName, logicInner, false, i, checkOverlaps);

        new G4LogicalBorderSurface("TiO2_Reflex", scintPhys, barPhys, surfTiO2);

        // ESCALONAMIENTO EN Z
        G4int subIndice = i % 4;
        if (subIndice >= 2) {
            subIndice = subIndice - 2;
        }

        G4double stackZ_A = (subIndice * 3.0 * mm);
        G4double stackZ_B = (subIndice * 3.0 * mm) - 1.5 * mm;

        G4double yDisplacement = -0.5 * fiber_extension;
        physAirRectoA[i] = new G4PVPlacement(rotTube,
            G4ThreeVector(xHole / 2., yDisplacement, stackZ_A),
            logicAirChannelA, "AirChannelA_P", localLogicScint, false, i, checkOverlaps);

        physAirRectoB[i] = new G4PVPlacement(rotTube,
            G4ThreeVector(-xHole / 2., yDisplacement, stackZ_B),
            logicAirChannelB, "AirChannelB_P", localLogicScint, false, i + 100, checkOverlaps);
    }


    // -----------------------------------------------------
    // 4. CAJA DETECTORA 
    // -----------------------------------------------------

    auto solidDetector = new G4Box("Detector_Solid", 0.5 * detector_x, 0.5 * detector_y, 0.5 * detector_z);

    auto logicDetector = new G4LogicalVolume(solidDetector, coreMat, "Detector_Logic");

    G4double yPosDetector = -100.0 * cm - fiber_extension - 0.5 * detector_y;

    new G4PVPlacement(nullptr,
        G4ThreeVector(0, yPosDetector, 0),
        logicDetector,
        "Detector_Phys",
        logicInner,
        false,
        0,
        checkOverlaps);

    fScoringVolume = logicDetector;

// -----------------------------------------------------
// 5. GIROS EN U ESCALONADOS 
// -----------------------------------------------------
    G4RotationMatrix* rotY = new G4RotationMatrix();
    rotY->rotateY(0 * deg);

    G4double radioGiroU = (2 * wrap_x) / 2.0;

    G4Torus* sGiroAir = new G4Torus("GiroAir_S", 0, channelDiam / 2., radioGiroU, 0, 180 * deg);
    G4LogicalVolume* lGiroAirA = new G4LogicalVolume(sGiroAir, world_mat, "GiroAirA_L");
    G4LogicalVolume* lGiroAirB = new G4LogicalVolume(sGiroAir, world_mat, "GiroAirB_L");

    G4Torus* sGiroOuterClad = new G4Torus("GiroOuterClad_S", 0, D / 2., radioGiroU, 0, 180 * deg);
    G4LogicalVolume* lGiroOuterClad = new G4LogicalVolume(sGiroOuterClad, fp, "GiroOuterClad_L");

    G4Torus* sGiroInnerClad = new G4Torus("GiroInnerClad_S", 0, innerDiam / 2., radioGiroU, 0, 180 * deg);
    G4LogicalVolume* lGiroInnerClad = new G4LogicalVolume(sGiroInnerClad, pmma, "GiroInnerClad_L");

    G4Torus* sGiroCore = new G4Torus("GiroCore_S", 0, coreDiam / 2., radioGiroU, 0, 180 * deg);
    G4LogicalVolume* lGiroCore = new G4LogicalVolume(sGiroCore, coreMat, "GiroCore_L");

    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), lGiroCore, "GiroCore_P", lGiroInnerClad, false, 0, checkOverlaps);
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), lGiroInnerClad, "GiroInnerClad_P", lGiroOuterClad, false, 0, checkOverlaps);

    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), lGiroOuterClad, "GiroFiberA_InsideAir_P", lGiroAirA, false, 0, checkOverlaps);
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), lGiroOuterClad, "GiroFiberB_InsideAir_P", lGiroAirB, false, 0, checkOverlaps);

    for (G4int g : {0, 1, 4, 5, 8, 9, 12, 13}) {

        G4double xBarraIni = -40.0 * cm + (g * wrap_x) + (0.5 * wrap_x);
        G4double xBarraFin = -40.0 * cm + ((g + 2) * wrap_x) + (0.5 * wrap_x);

        G4double xChannelA_Ini = xBarraIni + (xHole / 2.0);
        G4double xChannelA_Fin = xBarraFin + (xHole / 2.0);

        G4double xChannelB_Ini = xBarraIni - (xHole / 2.0);
        G4double xChannelB_Fin = xBarraFin - (xHole / 2.0);

        G4double xMidA = (xChannelA_Ini + xChannelA_Fin) / 2.0;
        G4double xMidB = (xChannelB_Ini + xChannelB_Fin) / 2.0;

        G4double yPos = 0.5 * bar_y;

        // --- ESCALONAMIENTO EN Z ---

        G4int subIndice = g % 4; 

        G4double stackZ_A = (subIndice * 3.0 * mm);
        G4double stackZ_B = (subIndice * 3.0 * mm) - 1.5 * mm; 

        // --- COLOCACIÓN DE LOS GIROS EN EL INNER LOGIC ---

        G4VPhysicalVolume* physGiroAirA = new G4PVPlacement(rotY,
            G4ThreeVector(xMidA, yPos, stackZ_A),
            lGiroAirA, "GiroAirA_P", logicInner, false, g, checkOverlaps);

        G4VPhysicalVolume* physGiroAirB = new G4PVPlacement(rotY,
            G4ThreeVector(xMidB, yPos, stackZ_B),
            lGiroAirB, "GiroAirB_P", logicInner, false, g + 100, checkOverlaps);
    
    }

    return physWorld;
}







