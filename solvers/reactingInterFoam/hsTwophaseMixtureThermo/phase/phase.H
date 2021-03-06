/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::phase

Description
    Single compressible phase derived from the phase-fraction.
    Used as part of the hsMultiphaseMixtureThermo for interface-capturing
    simulations in conjunction with a reaction subclass. The phase contains
    its constituitive elements (subspecies).

SourceFiles
    phase.C

\*---------------------------------------------------------------------------*/

#ifndef phase_H
#define phase_H

#include "volFields.H"
#include "PtrDictionary.H"
#include "dictionaryEntry.H"
#include "fvCFD.H"
#include "rhoChemistryCombustionModel.H"
#include "turbulenceModel.H"
#include "incompressible/viscosityModels/viscosityModel/viscosityModel.H"
#include "reactingMixture.H"
#include "reactionTypes.H"
#include "dimensionedScalar.H"
#include "dimensionedScalarFwd.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    //Forward declaration of subspecie class
    class mixturePhaseChangeModel;
    class subSpecie;
    
/*---------------------------------------------------------------------------*\
                           Class phase Declaration
\*---------------------------------------------------------------------------*/

class phase
:
    public volScalarField
{
    // Private data

        //- Phase name
        word name_;
        
        //- Phase dictionary (in thermophysicalProperties::phases)
        dictionary phaseDict_;
        
        //- Volume flux field for this phase
        surfaceScalarField rhoPhiAlpha_;
        
        //- Density of this phase
        volScalarField rhoAlpha_;
        
        //- Mass fraction sum of this phase
        volScalarField Ypsum_;
                
        //- Pointer to the other phase
        const phase* otherPhase_;
        
        //- Pointer to the top-level combustion model
        const combustionModels::rhoChemistryCombustionModel* combustionPtr_;

        //- Reference to the list of specie pointers
        PtrList<volScalarField>& species_;
        
        //- Reference to the list of specie thermodynamic data objects
        const PtrList<gasThermoPhysics>& speciesData_;
        
        //- Schmidt number
        dimensionedScalar Sc_;
        
        //- Dictionary of pointers to phase subspecies
        PtrDictionary<subSpecie> subSpecies_;

        //- Cell and face masks to define phase boundaries
        volScalarField cellMask_;
        surfaceScalarField faceMask_;
                

public:

    // Constructors

        //- Construct from components
        phase
        (
            const word& name,
            const dictionary& phaseDict,
            const fvMesh& mesh,
            PtrList<volScalarField>& species,
            const PtrList<gasThermoPhysics>& speciesData
        );

        //- Return clone
        autoPtr<phase> clone() const;


    // Member Functions

        const dictionary& dict() const
        {
            return phaseDict_;
        }

        const word& name() const
        {
            return name_;
        }

        const word& keyword() const
        {
            return name();
        }
        
        const volScalarField& cellMask() const
        {
            return cellMask_;
        }
        
        const PtrDictionary<subSpecie>& subSpecies() const
        {
            return subSpecies_;
        }
        
        PtrDictionary<subSpecie>& subSpecies()
        {
            return subSpecies_;
        }
                
        surfaceScalarField& rhoPhiAlpha()
        {
            return rhoPhiAlpha_;
        }
        
        const surfaceScalarField& rhoPhiAlpha() const
        {
            return rhoPhiAlpha_;
        }
        
        const surfaceScalarField& faceMask() const
        {
            return faceMask_;
        }
                
        const volScalarField& rhoAlpha() const
        {
            return rhoAlpha_;
        }
        
        void setOtherPhase(const phase* other);
        
        void setPhaseMasks
        (
            scalar maskTol,
            const volScalarField& p, 
            const volScalarField& T,
            const PtrDictionary<mixturePhaseChangeModel>& phaseChangeModels,
            const volScalarField& area
        );

        //- Return the phase density
        tmp<volScalarField> rho
        (
            const volScalarField& p, 
            const volScalarField& T
        ) const;

        //- Return the viscosity of the phase
        tmp<volScalarField> mu
        (
            const volScalarField& p, 
            const volScalarField& T
        ) const;
                  
        //- Return the phase compressibility
        tmp<volScalarField> psi
        (
            const volScalarField& T
        ) const;
                

        tmp<volScalarField> Yp() const;
        tmp<volScalarField> Ypp() const;
        tmp<volScalarField> Np() const;
        tmp<volScalarField> Npp() const;
        
        //- Get phase molar mass field
        tmp<volScalarField> W() const;
        
        tmp<volScalarField> kappa(const volScalarField& T) const;
        tmp<volScalarField> Cp(const volScalarField& T) const;
        tmp<volScalarField> Cv(const volScalarField& T) const;
              
        //- Correct the phase properties
        void correct(const volScalarField& p, const volScalarField& T);
        
        //- Read base transportProperties dictionary
        bool read(const dictionary& phaseDict);
                       
        void setCombustionPtr
        (
            combustionModels::rhoChemistryCombustionModel* combustion
        );
        
        void setSpecies(const volScalarField& rho);
        
        volScalarField& Y(const word& specie);
        
        const volScalarField& Y(const word& specie) const;
        
        tmp<volScalarField> sharp(scalar tol1, scalar tol2 = -1.0) const;
        
        //- Calculate the surface tension field for this phase
        tmp<volScalarField> sigma
        (
            const volScalarField& T,
            const volScalarField& kappaMask
        ) const;
                                        
        void calculateDs
        (
            const volScalarField& mut,
            const volScalarField& p,
            const volScalarField& T
        );

        tmp<surfaceScalarField> solveSubSpecies
        (
            const volScalarField& p,
            const volScalarField& T,
            const PtrDictionary<mixturePhaseChangeModel>& phaseChangeModels
        );
        
        void updateGlobalYs
        (
            const volScalarField& myRhoAlpha,
            const volScalarField& otherRhoAlpha
        );
        
        tmp<volScalarField> x(const word& name) const;
        tmp<volScalarField> xByY(const word& name) const;
        
        Pair<tmp<volScalarField> > YiSuSp
        (
            const subSpecie& specieI,
            const PtrDictionary<mixturePhaseChangeModel>& phaseChangeModels
        ) const;
        
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
