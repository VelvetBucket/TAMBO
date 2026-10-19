#include "DetectorConstruction.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"    // Para los canales y las fibras (cilindros)
#include "G4Torus.hh"   // Para los giros en U (toroides)
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4IonisParamMat.hh"
#include "G4SubtractionSolid.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"

void DetectorConstruction::BuildShield(
        G4LogicalVolume* motherVolume,
        G4double detectorHalfZ,
        G4double distanceSD)
{
    G4NistManager* nist = G4NistManager::Instance();

    // ------------------------------------------------------------------
    // Materials
    // ------------------------------------------------------------------
    if (!fMatPlexiglass) fMatPlexiglass = nist->FindOrBuildMaterial("G4_PLEXIGLASS");
    if (!fMatCellophane) fMatCellophane = nist->FindOrBuildMaterial("G4_CELLULOSE_CELLOPHANE");
    if (!fMatEpoxy) {
        G4Element* H = new G4Element("Hydrogen", "H", 1, 1.01*g/mole);
        G4Element* C = new G4Element("Carbon",   "C", 6, 12.01*g/mole);
        fMatEpoxy = new G4Material("Epoxy", 1.2*g/cm3, 2);
        fMatEpoxy->AddElement(H, 2);
        fMatEpoxy->AddElement(C, 2);
    }

    // ------------------------------------------------------------------
    // Derived dimensions
    // ------------------------------------------------------------------
    const G4double sourceHeight  = fRing_R1 - fThickShieldUp - fThickShieldDn;
    const G4double epoxyHeight   = fThickShieldUp;

    // Lowest point of the shield (relative to its own center)
    const G4double shieldLowestZ = sourceHeight/2. + fThickShieldDn;

    // Where the shield center must be so its lowest point is at
    // distanceSD above the detector's top face
    const G4double shieldCenterZ = detectorHalfZ + distanceSD - shieldLowestZ;

    // ------------------------------------------------------------------
    // Visual attributes
    // ------------------------------------------------------------------
    G4VisAttributes* tomato = new G4VisAttributes(G4Colour(1.00, 0.39, 0.28));
    G4VisAttributes* silver = new G4VisAttributes(G4Colour(0.75, 0.75, 0.75));
    G4VisAttributes* yellow = new G4VisAttributes(G4Colour(1.00, 1.00, 0.00));
    tomato->SetVisibility(true);
    silver->SetVisibility(true);
    yellow->SetVisibility(true);

    // ==================================================================
    // 1. Outer ring (Plexiglass side wall)
    // ==================================================================
    G4bool checkOverlaps = true;
    G4Tubs* solidOuterRing = new G4Tubs("SrCs_Sshield_out",
        fRing_R1, fRing_R2, sourceHeight/2., 0., twopi);

    G4LogicalVolume* logicOuterRing = new G4LogicalVolume(
        solidOuterRing, fMatPlexiglass, "SrCs_Sshield_out");

    new G4PVPlacement(0,
        G4ThreeVector(0, 0, shieldCenterZ),
        logicOuterRing, "SrCs_Sshield_out",
        motherVolume, false, 0, checkOverlaps);
    logicOuterRing->SetVisAttributes(tomato);

    // ==================================================================
    // 2. Epoxy cap (bottom of source cavity)
    // ==================================================================
    G4Tubs* solidEpoxyDown = new G4Tubs("SrCs_epoxy_Sshield_down",
        fRing_R1/2., fRing_R1, sourceHeight/2., 0., twopi);

    G4LogicalVolume* logicEpoxyDown = new G4LogicalVolume(
        solidEpoxyDown, fMatEpoxy, "SrCs_epoxy_Sshield_down");

    new G4PVPlacement(0,
        G4ThreeVector(0, 0, shieldCenterZ),
        logicEpoxyDown, "SrCs_epoxy_Sshield_down",
        motherVolume, false, 0, checkOverlaps);
    logicEpoxyDown->SetVisAttributes(silver);

    // ==================================================================
    // 3. Epoxy shield (top)
    // ==================================================================
    G4Tubs* solidEpoxyUp = new G4Tubs("SrCs_epoxy_Sshield_up",
        0., fRing_R1, epoxyHeight/2., 0., twopi);

    G4LogicalVolume* logicEpoxyUp = new G4LogicalVolume(
        solidEpoxyUp, fMatEpoxy, "SrCs_epoxy_Sshield_up");

    new G4PVPlacement(0,
        G4ThreeVector(0, 0, shieldCenterZ - epoxyHeight/2. - sourceHeight/2.),
        logicEpoxyUp, "SrCs_epoxy_Sshield_up",
        motherVolume, false, 0, checkOverlaps);
    logicEpoxyUp->SetVisAttributes(silver);

    // ==================================================================
    // 4. Plexiglass top cap
    // ==================================================================
    if (fThickShieldUp > 0.*um) {
        G4Tubs* solidCapUp = new G4Tubs("SrCs_tub_Sshield_up",
            fRing_R1, fRing_R2, fThickShieldUp/2., 0., twopi);

        G4LogicalVolume* logicCapUp = new G4LogicalVolume(
            solidCapUp, fMatPlexiglass, "SrCs_tub_Sshield_up");

        new G4PVPlacement(0,
            G4ThreeVector(0, 0, shieldCenterZ - sourceHeight/2. - fThickShieldUp/2.),
            logicCapUp, "SrCs_tub_Sshield_up",
            motherVolume, false, 0, checkOverlaps);
        logicCapUp->SetVisAttributes(tomato);
    }

    // ==================================================================
    // 5. Cellophane paper (top)
    // ==================================================================
    {
        G4Tubs* solidPaper = new G4Tubs("SrCs_paper_Sshield_up",
            0., fRing_R2, fThickPaper/2., 0., twopi);

        G4LogicalVolume* logicPaper = new G4LogicalVolume(
            solidPaper, fMatCellophane, "SrCs_paper_Sshield_up");

        new G4PVPlacement(0,
            G4ThreeVector(0, 0,
                shieldCenterZ - fThickShieldUp - sourceHeight/2. - fThickPaper/2.),
            logicPaper, "SrCs_paper_Sshield_up",
            motherVolume, false, 0, checkOverlaps);
        logicPaper->SetVisAttributes(yellow);
    }

    // ==================================================================
    // 6. Plexiglass bottom cap (closest to detector)
    // ==================================================================
    if (fThickShieldDn > 0.*um) {
        G4Tubs* solidCapDown = new G4Tubs("SrCs_cap_Sshield_down",
            0., fRing_R2, fThickShieldDn/2., 0., twopi);

        G4LogicalVolume* logicCapDown = new G4LogicalVolume(
            solidCapDown, fMatPlexiglass, "SrCs_cap_Sshield_down");

        new G4PVPlacement(0,
            G4ThreeVector(0, 0, shieldCenterZ + sourceHeight/2. + fThickShieldDn/2.),
            logicCapDown, "SrCs_cap_Sshield_down",
            motherVolume, false, 0, checkOverlaps);
        logicCapDown->SetVisAttributes(tomato);
    }
    
    G4cout << "\n============================================================" << G4endl;
    G4cout << "  Shield Positioned Above Detector" << G4endl;
    G4cout << "============================================================\n" << G4endl;
}

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
    // 1. MATERIALES (USANDO PUNTEROS NIST)
    // -----------------------------------------------------
    G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* al_mat = nist->FindOrBuildMaterial("G4_Al");
    G4MaterialPropertiesTable* mptAir = new G4MaterialPropertiesTable();
    G4double airRIndex[] = { 1.0, 1.0 };
    G4double airEnergy[] = { 1.0 * eV, 5.0 * eV };
    mptAir->AddProperty("RINDEX", airEnergy, airRIndex, 2);
    world_mat->SetMaterialPropertiesTable(mptAir);

    // Puntero directo al material de tu lista
    G4Material* scintMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

    // -----------------------------------------------------
    // 1. PROPIEDADES ÓPTICAS DEL VINILTOLUENO (BC-408)
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
    mptScint->AddConstProperty("SCINTILLATIONYIELD", 10000. / MeV);
    mptScint->AddConstProperty("RESOLUTIONSCALE", 1.0);
    mptScint->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1 * ns);
    mptScint->AddConstProperty("SCINTILLATIONYIELD1", 1.0);

    scintMat->SetMaterialPropertiesTable(mptScint);
    // -----------------------------------------------------

    // Material Base del Núcleo (Core)
    // -----------------------------------------------------
    // MATERIALES WLS
    // -----------------------------------------------------
    G4Material* coreMat = nist->FindOrBuildMaterial("G4_POLYSTYRENE");
    G4MaterialPropertiesTable* mptWLS = new G4MaterialPropertiesTable();

    // ====================================================================
    // CONFIGURACIÓN ÓPTICA COMPLETA DE LA FIBRA WLS (KURARAY Y-11(200))
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

    // Tiempo de decaimiento característico de la fibra Y-11 de Kuraray (7.2 nanosegundos)
    mptWLS->AddConstProperty("WLSTIMECONSTANT", 7.2 * ns);

    // Finalmente, asignamos la tabla al material de tu fibra (ej. fibraMaterial)
    // fibraMaterial->SetMaterialPropertiesTable(mptWLS);

    coreMat->SetMaterialPropertiesTable(mptWLS);

    // --- MATERIALES PARA MULTI CLADDING ---
    G4Material* pmma = nist->FindOrBuildMaterial("G4_PLEXIGLASS"); // Inner Cladding (n=1.49)
    G4Element* elF = nist->FindOrBuildElement("F");
    G4Element* elC = nist->FindOrBuildElement("C");
    G4Material* fp = new G4Material("FluorinatedPolymer", 1.43 * g / cm3, 2); // Outer Cladding (n=1.42)
    fp->AddElement(elC, 2); fp->AddElement(elF, 4); // Composición simplificada

    // Propiedades Ópticas
    G4MaterialPropertiesTable* mptInner = new G4MaterialPropertiesTable();
    mptInner->AddProperty("RINDEX", wlsEnergy, rIndexInner, nWls);
    pmma->SetMaterialPropertiesTable(mptInner);

    G4MaterialPropertiesTable* mptOuter = new G4MaterialPropertiesTable();
    mptOuter->AddProperty("RINDEX", wlsEnergy, rIndexOuter, nWls);
    fp->SetMaterialPropertiesTable(mptOuter);

    // -----------------------------------------------------
    // 2. SUPERFICIE REFLECTANTE (Pintura TiO2)
    // -----------------------------------------------------
    // Esto define cómo rebota la luz cuando toca la pintura blanca
    G4OpticalSurface* surfTiO2 = new G4OpticalSurface("TiO2Surface");
    surfTiO2->SetType(dielectric_metal);
    surfTiO2->SetFinish(ground); // Reflexión difusa (como pintura mate)
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
    //G4double al_y = 110.0 * inch;
    G4double al_z = 1.5 * inch;
    G4double al_wall_thick = 0.050 * inch;

    G4double bar_x = 5.0 * cm;
    G4double bar_y = 200.0 * cm;
    G4double bar_z = 1.0 * cm;
    G4double paint_thick = 0.25 * mm;
    G4double capThick = paint_thick;

    G4double channelDiam = 2.5 * mm;

    // 2. GEOMETRÍA DE LA FIBRA (Definición lógica)
    G4double D = 1.0 * mm;
    G4double coreDiam = 0.88 * mm; // Según 6% de reducción total
    G4double innerDiam = 0.94 * mm;
    G4double fiber_extension = 20.0 * cm;

    // --- NUEVO: DIMENSIONES DE LA CAJA DEL DETECTOR ---
    G4double detector_x = 2.0 * mm;       // Cubre todo el ancho del arreglo de aluminio
    G4double detector_y = 2.0 * mm;   // Espesor/grosor de la caja (4 cm es ideal)
    G4double detector_z = 2.0 * mm;       // Cubre todo el espesor en Z


    G4double innerChannelHalf = 0.5 * bar_y;          // tramo DENTRO de la barra
    G4double extChannelHalf = 0.5 * fiber_extension; // tramo de extensión, fuera de la barra
    G4double radioGiroU = (2 * (bar_x + 2 * paint_thick)) / 2.0;

    // Limites reales de la geometría interna (en Y local)
    G4double max_extent_y = 0.5 * bar_y + capThick + fiber_extension + detector_y;
    G4double margin_y = 2.0 * cm; // Margen de aire de seguridad


    // Altura interna y externa de la caja recortada
    G4double inner_y = 2.0 * max_extent_y + 2.0 * margin_y;
    G4double al_y = inner_y + 2 * al_wall_thick;

    // Desplazamiento del centro de la caja en Y para envolver la asimetría
    //G4double yBoxPos = (y_max + y_min) / 2.0; // ~ -7.48 cm

    // --- Fibra multi-clad para el tramo DENTRO de la barra ---
    G4Tubs* sOuterClad_in = new G4Tubs("OuterClad_in_S", 0, D / 2., innerChannelHalf, 0, 360 * deg);
    G4LogicalVolume* lOuterClad_in = new G4LogicalVolume(sOuterClad_in, fp, "OuterClad_in_L");
    G4Tubs* sInnerClad_in = new G4Tubs("InnerClad_in_S", 0, innerDiam / 2., innerChannelHalf, 0, 360 * deg);
    G4LogicalVolume* lInnerClad_in = new G4LogicalVolume(sInnerClad_in, pmma, "InnerClad_in_L");
    G4Tubs* sCore_in = new G4Tubs("Core_in_S", 0, coreDiam / 2., innerChannelHalf, 0, 360 * deg);
    G4LogicalVolume* lCore_in = new G4LogicalVolume(sCore_in, coreMat, "Core_in_L");
    new G4PVPlacement(nullptr, G4ThreeVector(), lCore_in, "Core_in_P", lInnerClad_in, false, 0, checkOverlaps);
    new G4PVPlacement(nullptr, G4ThreeVector(), lInnerClad_in, "InnerClad_in_P", lOuterClad_in, false, 0, checkOverlaps);

    // --- Fibra multi-clad para el tramo de EXTENSIÓN (hasta el detector) ---
    G4Tubs* sOuterClad_ext = new G4Tubs("OuterClad_ext_S", 0, D / 2., extChannelHalf, 0, 360 * deg);
    G4LogicalVolume* lOuterClad_ext = new G4LogicalVolume(sOuterClad_ext, fp, "OuterClad_ext_L");
    G4Tubs* sInnerClad_ext = new G4Tubs("InnerClad_ext_S", 0, innerDiam / 2., extChannelHalf, 0, 360 * deg);
    G4LogicalVolume* lInnerClad_ext = new G4LogicalVolume(sInnerClad_ext, pmma, "InnerClad_ext_L");
    G4Tubs* sCore_ext = new G4Tubs("Core_ext_S", 0, coreDiam / 2., extChannelHalf, 0, 360 * deg);
    G4LogicalVolume* lCore_ext = new G4LogicalVolume(sCore_ext, coreMat, "Core_ext_L");
    new G4PVPlacement(nullptr, G4ThreeVector(), lCore_ext, "Core_ext_P", lInnerClad_ext, false, 0, checkOverlaps);
    new G4PVPlacement(nullptr, G4ThreeVector(), lInnerClad_ext, "InnerClad_ext_P", lOuterClad_ext, false, 0, checkOverlaps);

    //G4double half_fiber_length = 0.5 * bar_y + 0.5 * fiber_extension;
    //G4Tubs* sOuterClad = new G4Tubs("OuterClad_S", 0, D / 2.,half_fiber_length, 0, 360 * deg);
    //G4LogicalVolume* lOuterClad = new G4LogicalVolume(sOuterClad, fp, "OuterClad_L");

    //G4Tubs* sInnerClad = new G4Tubs("InnerClad_S", 0, innerDiam / 2.,  half_fiber_length, 0, 360 * deg);
    //G4LogicalVolume* lInnerClad = new G4LogicalVolume(sInnerClad, pmma, "InnerClad_L");

    //G4Tubs* sCore = new G4Tubs("Core_S", 0, coreDiam / 2.,half_fiber_length, 0, 360 * deg);
    //G4LogicalVolume* lCore = new G4LogicalVolume(sCore, coreMat, "Core_L");


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

    // AIRE INTERNO (Capa madre para las barras)
    G4double inner_x = al_x - 2 * al_wall_thick;
    //G4double inner_y = al_y - 2 * al_wall_thick;
    G4double inner_z = al_z - 2 * al_wall_thick;
    auto solidInner = new G4Box("Inner_Solid", 0.5 * inner_x, 0.5 * inner_y, 0.5 * inner_z);
    auto logicInner = new G4LogicalVolume(solidInner, world_mat, "Inner_Logic");
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logicInner, "Inner_Phys", logicAlBox, false, 0, checkOverlaps);

    // PINTURA TiO2 (Wrap) - MARCO HUECO ABIERTO EN LAS TAPAS
    G4double wrap_x = bar_x + 2 * paint_thick;
    G4double wrap_y = bar_y; // Mismo largo que la barra para liberar las tapas
    G4double wrap_z = bar_z + 2 * paint_thick;

    // 1. Caja exterior de la pintura
    auto solidWrapOuter = new G4Box("Wrap_Outer_Solid", 0.5 * wrap_x, 0.5 * wrap_y, 0.5 * wrap_z);

    // 2. Caja interior para perforar
    auto solidWrapInner = new G4Box("Wrap_Inner_Solid", 0.5 * bar_x, 0.5 * wrap_y + 1.0 * mm, 0.5 * bar_z);

    // 3. Restamos el interior para crear el marco hueco
    auto solidWrap = new G4SubtractionSolid("Wrap_Solid", solidWrapOuter, solidWrapInner);

    // 4. Creamos el volumen lógico usando el nuevo sólido ya inicializado
    auto logicWrap = new G4LogicalVolume(solidWrap, TiO2, "Wrap_Logic");

    // PLÁSTICO (Scintillator Sólido)
    auto solidScint = new G4Box("Scint_Solid", 0.5 * bar_x, 0.5 * bar_y, 0.5 * bar_z);

    // --- DIMENSIONES DE PERFORACIÓN ---
    //G4double channelDiam = 2.5 * mm;
    G4double fiberDiam = 1.0 * mm;
    G4double xHole = 1.2 * cm; // Distancia entre el Agujero A y el Agujero B



    // Canales DENTRO de la barra (ahora sí contenidos correctamente: mismo largo
    // que la barra, centrados en y=0 local)
    G4Tubs* solidAirChannel_in = new G4Tubs("AirChannel_in_S", 0, channelDiam / 2., innerChannelHalf, 0, 360 * deg);
    G4LogicalVolume* logicAirChannelA_in = new G4LogicalVolume(solidAirChannel_in, world_mat, "AirChannelA_in_L");
    G4LogicalVolume* logicAirChannelB_in = new G4LogicalVolume(solidAirChannel_in, world_mat, "AirChannelB_in_L");
    new G4PVPlacement(nullptr, G4ThreeVector(), lOuterClad_in, "FiberA_in_InsideAir_P", logicAirChannelA_in, false, 0, checkOverlaps);
    new G4PVPlacement(nullptr, G4ThreeVector(), lOuterClad_in, "FiberB_in_InsideAir_P", logicAirChannelB_in, false, 0, checkOverlaps);

    // Canales de EXTENSIÓN (ahora hijos de logicInner, NO de la barra)
    G4Tubs* solidAirChannel_ext = new G4Tubs("AirChannel_ext_S", 0, channelDiam / 2., extChannelHalf, 0, 360 * deg);
    G4LogicalVolume* logicAirChannelA_ext = new G4LogicalVolume(solidAirChannel_ext, world_mat, "AirChannelA_ext_L");
    G4LogicalVolume* logicAirChannelB_ext = new G4LogicalVolume(solidAirChannel_ext, world_mat, "AirChannelB_ext_L");
    new G4PVPlacement(nullptr, G4ThreeVector(), lOuterClad_ext, "FiberA_ext_InsideAir_P", logicAirChannelA_ext, false, 0, checkOverlaps);
    new G4PVPlacement(nullptr, G4ThreeVector(), lOuterClad_ext, "FiberB_ext_InsideAir_P", logicAirChannelB_ext, false, 0, checkOverlaps);

    G4RotationMatrix* rotTube = new G4RotationMatrix();
    rotTube->rotateX(90 * deg);

    //** NUEVO: sólido "plantilla" del agujero de la tapa TiO2 (se usa 2 veces por tapa) **
    G4Tubs* solidCapHole = new G4Tubs("CapHole_S", 0, channelDiam / 2., capThick / 2. + 1.0 * mm, 0, 360 * deg);
    G4double capChannelHalf = capThick / 2.;
    G4Tubs* sOuterClad_cap = new G4Tubs("OuterClad_cap_S", 0, D / 2., capChannelHalf, 0, 360 * deg);
    G4LogicalVolume* lOuterClad_cap = new G4LogicalVolume(sOuterClad_cap, fp, "OuterClad_cap_L");
    G4Tubs* sInnerClad_cap = new G4Tubs("InnerClad_cap_S", 0, innerDiam / 2., capChannelHalf, 0, 360 * deg);
    G4LogicalVolume* lInnerClad_cap = new G4LogicalVolume(sInnerClad_cap, pmma, "InnerClad_cap_L");
    G4Tubs* sCore_cap = new G4Tubs("Core_cap_S", 0, coreDiam / 2., capChannelHalf, 0, 360 * deg);
    G4LogicalVolume* lCore_cap = new G4LogicalVolume(sCore_cap, coreMat, "Core_cap_L");
    new G4PVPlacement(nullptr, G4ThreeVector(), lCore_cap, "Core_cap_P", lInnerClad_cap, false, 0, checkOverlaps);
    new G4PVPlacement(nullptr, G4ThreeVector(), lInnerClad_cap, "InnerClad_cap_P", lOuterClad_cap, false, 0, checkOverlaps);



    /*// --- 1. LOS CANALES DE AIRE (Los "Agujeros") ---
    G4Tubs* solidAirChannel = new G4Tubs("AirChannel_S", 0, channelDiam / 2.,half_fiber_length, 0, 360 * deg);
    G4LogicalVolume* logicAirChannelA = new G4LogicalVolume(solidAirChannel, world_mat, "AirChannelA_L");
    G4LogicalVolume* logicAirChannelB = new G4LogicalVolume(solidAirChannel, world_mat, "AirChannelB_L");

    // --- 2. ENSAMBLAJE DE LA FIBRA MULTI-CLAD (Matrioshka) ---
    new G4PVPlacement(nullptr, G4ThreeVector(), lCore, "Core_P", lInnerClad, false, 0, checkOverlaps);
    new G4PVPlacement(nullptr, G4ThreeVector(), lInnerClad, "InnerClad_P", lOuterClad, false, 0, checkOverlaps);

    // --- 3. METER LAS FIBRAS RECTAS DENTRO DE SUS CANALES DE AIRE ---
    new G4PVPlacement(nullptr, G4ThreeVector(), lOuterClad, "FiberA_InsideAir_P", logicAirChannelA, false, 0, checkOverlaps);
    new G4PVPlacement(nullptr, G4ThreeVector(), lOuterClad, "FiberB_InsideAir_P", logicAirChannelB, false, 0, checkOverlaps);
    
    G4RotationMatrix* rotTube = new G4RotationMatrix();
    rotTube->rotateX(90 * deg);
    G4LogicalVolume* scoringScintLogic = nullptr;
    */

    // -----------------------------------------------------
// 4. CONSTRUCCIÓN DE LA CAJA DETECTORA (READOUT BOX - SILICIO)
// -----------------------------------------------------

    // A) Buscamos el material Silicio en la base de datos NIST
    G4Material* siliconMat = nist->FindOrBuildMaterial("G4_Si");

    // B) Le añadimos propiedades ópticas básicas para que interactúe con los fotones ópticos
    G4MaterialPropertiesTable* mptSilicon = new G4MaterialPropertiesTable();

    // Rango de energía básico para cubrir tu espectro verde (~2 eV a 3.5 eV)
    G4double siEnergy[] = { 1.5 * eV, 4.5 * eV };

    // El silicio tiene un índice de refracción muy alto en el visible (aprox. 3.4 a 4.0)
    G4double siRIndex[] = { 3.4, 4.0 };

    // Longitud de absorción extremadamente corta,
    G4double siAbsLength[] = { 0.001 * mm, 0.001 * mm };

    mptSilicon->AddProperty("RINDEX", siEnergy, siRIndex, 2);
    mptSilicon->AddProperty("ABSLENGTH", siEnergy, siAbsLength, 2);
    siliconMat->SetMaterialPropertiesTable(mptSilicon);

    // 1. Definimos el sólido (una caja tridimensional)
    auto solidDetector = new G4Box("Detector_Solid", 0.5 * detector_x, 0.5 * detector_y, 0.5 * detector_z);

    // 2. Creamos el volumen lógico usando el NUEVO material de Silicio
    auto logicDetector = new G4LogicalVolume(solidDetector, siliconMat, "Detector_Logic");

    // 3. Calculamos la posición exacta en Y:
    // El aluminio interno termina en '-0.5 * inner_y'. A eso le restamos los 5 cm de fibra al aire, 
    // y le restamos la mitad del grosor de la caja para que su borde superior toque las puntas de las fibras.
    G4double yPosDetector = -0.5 * bar_y - capThick - fiber_extension - 0.5 * detector_y;

    // 4. Colocamos físicamente la caja dentro del volumen madre interno (logicInner)
    //new G4PVPlacement(nullptr,
    //    G4ThreeVector(0, yPosDetector, 0),
    //    logicDetector,
    //    "Detector_Phys",
    //    logicInner,
    //    false,
     //   0,
     //   checkOverlaps);

    // 5. REGISTRO COMO SCORING VOLUME
    // Le decimos a Geant4 que este volumen completo registrará los impactos de los fotones

    // -----------------------------------------------------
    // COLOCACIÓN DE LAS 16 BARRAS INDEPENDIENTES
    // -----------------------------------------------------
    for (G4int i = 0; i < 16; i++) {
        G4double xPos = -40.0 * cm + (i * wrap_x) + (0.5 * wrap_x);

        // 1. CREAMOS UN LÓGICO DE PINTURA EXCLUSIVO PARA ESTA BARRA 'i'
        G4String wrapName = "Wrap_Logic_" + std::to_string(i);
        G4LogicalVolume* localLogicWrap = new G4LogicalVolume(solidWrap, TiO2, wrapName);

        // 2. CREAMOS UN LÓGICO DE PLÁSTICO EXCLUSIVO PARA ESTA BARRA 'i'
        G4String scintName = "Scint_Logic_" + std::to_string(i);
        G4LogicalVolume* localLogicScint = new G4LogicalVolume(solidScint, scintMat, scintName);
        localLogicScint->GetMaterial()->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);

        // Si es la primera barra, la guardamos para el Scoring Volume global
        if (i == 0) { fScoringVolume = localLogicScint; }

        // 3. Colocamos el plástico único dentro de SU pintura única
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


        //** CAMBIO: canal + fibra DENTRO de la barra, ahora perfectamente
        //   contenido (mismo largo que la barra, centrado en y=0 local) **
        new G4PVPlacement(rotTube,
            G4ThreeVector(xHole / 2., 0, stackZ_A), logicAirChannelA_in,
            "AirChannelA_in_P_" + std::to_string(i), localLogicScint, false, i, checkOverlaps);
        new G4PVPlacement(rotTube,
            G4ThreeVector(-xHole / 2., 0, stackZ_B), logicAirChannelB_in,
            "AirChannelB_in_P_" + std::to_string(i), localLogicScint, false, i + 100, checkOverlaps);

        //** NUEVO: TAPA TiO2 (con sus 2 agujeros para las fibras), reutilizada
        //   arriba y abajo -- sella las caras que antes quedaban abiertas **
        auto solidCapOuter = new G4Box("CapOuter_S_" + std::to_string(i), 0.5 * wrap_x, 0.5 * capThick, 0.5 * wrap_z);
        auto solidCapCut1 = new G4SubtractionSolid("CapCut1_S_" + std::to_string(i), solidCapOuter, solidCapHole,
            rotTube, G4ThreeVector(xHole / 2., 0, stackZ_A));
        auto solidCapFinal = new G4SubtractionSolid("Cap_S_" + std::to_string(i), solidCapCut1, solidCapHole,
            rotTube, G4ThreeVector(-xHole / 2., 0, stackZ_B));
        auto logicCap = new G4LogicalVolume(solidCapFinal, TiO2, "Cap_Logic_" + std::to_string(i));

        // tapa de abajo (hacia el detector)
        G4VPhysicalVolume* capBottomPhys = new G4PVPlacement(nullptr,
            G4ThreeVector(xPos, -0.5 * bar_y - 0.5 * capThick, 0), logicCap,
            "CapBottom_Phys_" + std::to_string(i), logicInner, false, i, checkOverlaps);
        new G4LogicalBorderSurface("TiO2_CapBottom_" + std::to_string(i), scintPhys, capBottomPhys, surfTiO2);

        // tapa de arriba (hacia el giro en U)
        G4VPhysicalVolume* capTopPhys = new G4PVPlacement(nullptr,
            G4ThreeVector(xPos, 0.5 * bar_y + 0.5 * capThick, 0), logicCap,
            "CapTop_Phys_" + std::to_string(i), logicInner, false, i + 100, checkOverlaps);
        new G4LogicalBorderSurface("TiO2_CapTop_" + std::to_string(i), scintPhys, capTopPhys, surfTiO2);


        new G4PVPlacement(rotTube,
            G4ThreeVector(xPos + xHole / 2., -0.5 * bar_y - 0.5 * capThick, stackZ_A), lOuterClad_cap,
            "SpliceBottomA_P_" + std::to_string(i), logicInner, false, i, checkOverlaps);
        new G4PVPlacement(rotTube,
            G4ThreeVector(xPos - xHole / 2., -0.5 * bar_y - 0.5 * capThick, stackZ_B), lOuterClad_cap,
            "SpliceBottomB_P_" + std::to_string(i), logicInner, false, i + 100, checkOverlaps);
        new G4PVPlacement(rotTube,
            G4ThreeVector(xPos + xHole / 2., 0.5 * bar_y + 0.5 * capThick, stackZ_A), lOuterClad_cap,
            "SpliceTopA_P_" + std::to_string(i), logicInner, false, i, checkOverlaps);
        new G4PVPlacement(rotTube,
            G4ThreeVector(xPos - xHole / 2., 0.5 * bar_y + 0.5 * capThick, stackZ_B), lOuterClad_cap,
            "SpliceTopB_P_" + std::to_string(i), logicInner, false, i + 100, checkOverlaps);


        //** NUEVO: canal + fibra de EXTENSIÓN, ahora correctamente contenido en
        //   logicInner (no como hijo de la barra) **
        G4double yExtCenter = -0.5 * bar_y - capThick - extChannelHalf;
        new G4PVPlacement(rotTube,
            G4ThreeVector(xPos + xHole / 2., yExtCenter, stackZ_A), logicAirChannelA_ext,
            "AirChannelA_ext_P_" + std::to_string(i), logicInner, false, i, checkOverlaps);
        new G4PVPlacement(rotTube,
            G4ThreeVector(xPos - xHole / 2., yExtCenter, stackZ_B), logicAirChannelB_ext,
            "AirChannelB_ext_P_" + std::to_string(i), logicInner, false, i + 100, checkOverlaps);


        // Colocar un SiPM alineado a la Fibra A
        new G4PVPlacement(nullptr,
            G4ThreeVector(xPos + xHole / 2., yPosDetector, stackZ_A),
            logicDetector,
            "SiPM_A_Phys",
            logicInner,
            false,
            i, // Copy number único
            checkOverlaps);

        // Colocar un SiPM alineado a la Fibra B
        new G4PVPlacement(nullptr,
            G4ThreeVector(xPos - xHole / 2., yPosDetector, stackZ_B),
            logicDetector,
            "SiPM_B_Phys",
            logicInner,
            false,
            i + 100, // Copy number único
            checkOverlaps);


        /*// 6. Perforamos ÚNICAMENTE este plástico local con sus dos canales
        G4double yDisplacement = -0.5 * fiber_extension;
        physAirRectoA[i] = new G4PVPlacement(rotTube,
            G4ThreeVector(xHole / 2., yDisplacement, stackZ_A),
            logicAirChannelA, "AirChannelA_P", localLogicScint, false, i, checkOverlaps);

        physAirRectoB[i] = new G4PVPlacement(rotTube,
            G4ThreeVector(-xHole / 2., yDisplacement, stackZ_B),
            logicAirChannelB, "AirChannelB_P", localLogicScint, false, i + 100, checkOverlaps);

            */
            
    }

// -----------------------------------------------------
// 5. GIROS EN U ESCALONADOS (CON MULTI-CLADDING CORREGIDO)
// -----------------------------------------------------
    G4RotationMatrix* rotY = new G4RotationMatrix();
    rotY->rotateY(0 * deg);

    //G4double radioGiroU = (2 * wrap_x) / 2.0;

    // 1. Toroide contenedor: CANAL DE AIRE CURVO
    G4Torus* sGiroAir = new G4Torus("GiroAir_S", 0, channelDiam / 2., radioGiroU, 0, 180 * deg);
    G4LogicalVolume* lGiroAirA = new G4LogicalVolume(sGiroAir, world_mat, "GiroAirA_L");
    G4LogicalVolume* lGiroAirB = new G4LogicalVolume(sGiroAir, world_mat, "GiroAirB_L");

    // 2. Toroide para el REVESTIMIENTO EXTERIOR (Fluorinated Polymer - D = 1.0 mm)
    G4Torus* sGiroOuterClad = new G4Torus("GiroOuterClad_S", 0, D / 2., radioGiroU, 0, 180 * deg);
    G4LogicalVolume* lGiroOuterClad = new G4LogicalVolume(sGiroOuterClad, fp, "GiroOuterClad_L");

    // 3. Toroide para el REVESTIMIENTO INTERIOR (PMMA - innerDiam = 0.94 mm)
    G4Torus* sGiroInnerClad = new G4Torus("GiroInnerClad_S", 0, innerDiam / 2., radioGiroU, 0, 180 * deg);
    G4LogicalVolume* lGiroInnerClad = new G4LogicalVolume(sGiroInnerClad, pmma, "GiroInnerClad_L");

    // 4. Toroide para el NÚCLEO (Polystyrene - coreDiam = 0.88 mm)
    G4Torus* sGiroCore = new G4Torus("GiroCore_S", 0, coreDiam / 2., radioGiroU, 0, 180 * deg);
    G4LogicalVolume* lGiroCore = new G4LogicalVolume(sGiroCore, coreMat, "GiroCore_L");

    // Ensamble Matrioshka Curvo: Núcleo -> InnerClad -> OuterClad
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), lGiroCore, "GiroCore_P", lGiroInnerClad, false, 0, checkOverlaps);
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), lGiroInnerClad, "GiroInnerClad_P", lGiroOuterClad, false, 0, checkOverlaps);

    // Metemos la fibra curva completa dentro de los dos canales de aire (A y B)
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), lGiroOuterClad, "GiroFiberA_InsideAir_P", lGiroAirA, false, 0, checkOverlaps);
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), lGiroOuterClad, "GiroFiberB_InsideAir_P", lGiroAirB, false, 0, checkOverlaps);

    // BUCLE ESCALONADO: Conexiones por bloques de 4 barras
    // Bloque 1: barras 0, 1 (conectan a 2, 3)
    // Bloque 2: barras 4, 5 (conectan a 6, 7)
    // Bloque 3: barras 8, 9 (conectan a 10, 11)
    // Bloque 4: barras 12, 13 (conectan a 14, 15)
    for (G4int g : {0, 1, 4, 5, 8, 9, 12, 13}) {

        // Posiciones en X de los centros de las barras involucradas
        G4double xBarraIni = -40.0 * cm + (g * wrap_x) + (0.5 * wrap_x);
        G4double xBarraFin = -40.0 * cm + ((g + 2) * wrap_x) + (0.5 * wrap_x);

        // Posiciones reales en X de los canales A y B
        G4double xChannelA_Ini = xBarraIni + (xHole / 2.0);
        G4double xChannelA_Fin = xBarraFin + (xHole / 2.0);

        G4double xChannelB_Ini = xBarraIni - (xHole / 2.0);
        G4double xChannelB_Fin = xBarraFin - (xHole / 2.0);

        // Puntos medios en X para centrar el Toro de cada canal
        G4double xMidA = (xChannelA_Ini + xChannelA_Fin) / 2.0;
        G4double xMidB = (xChannelB_Ini + xChannelB_Fin) / 2.0;

        // Altura (Y): Justo en el extremo superior de la barra
        G4double yPos = 0.5 * bar_y;

        // --- ESCALONAMIENTO EN Z PARA EVITAR CHOQUES ---
        // Usamos el índice de la barra local del bloque (g % 4) para darle una altura distinta en Z.
        // Así, el giro de la barra 0 va a una profundidad, y el de la barra 1 va más adelante/atrás.
        G4int subIndice = g % 4; // Esto devolverá 0 o 1 para tus conexiones

        // Separación de 3.0 mm en el eje Z por cada paso del escalonamiento
        G4double stackZ_A = (subIndice * 3.0 * mm);
        G4double stackZ_B = (subIndice * 3.0 * mm) - 1.5 * mm; // Desplazamiento extra entre canal A y B para seguridad

        // --- COLOCACIÓN DE LOS GIROS EN EL INNER LOGIC ---

        // // 1. Colocamos el Giro A y guardamos su volumen físico
        G4VPhysicalVolume* physGiroAirA = new G4PVPlacement(rotY,
            G4ThreeVector(xMidA, yPos, stackZ_A),
            lGiroAirA, "GiroAirA_P", logicInner, false, g, checkOverlaps);

        // 2. Colocamos el Giro B y guardamos su volumen físico
        G4VPhysicalVolume* physGiroAirB = new G4PVPlacement(rotY,
            G4ThreeVector(xMidB, yPos, stackZ_B),
            lGiroAirB, "GiroAirB_P", logicInner, false, g + 100, checkOverlaps);
    
    }

// -----------------------------------------------------
// 6. POSICIONANDO LA FUENTE (SHIELD) EN RELACION A LA CAJA 
// -----------------------------------------------------
                
      G4double fDistance_SD = 0. * mm;

      // Call the shield builder:
      BuildShield(logicWorld, 0.5 * al_z, fDistance_SD);

    fScoringVolume = logicDetector;
    return physWorld;
}
