using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlueEnergy : Energy
{
    protected override void Awake()
    {
        base.Awake();
        energyType = EnergyType.BlueEnergy;
        _objectCode = O.O_BLUE_ENERGY;
    }
    protected override void Start()
    {
        base.Start();
    }
}
