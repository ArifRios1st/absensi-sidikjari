<?php

namespace App\Enums;

use App\Traits\EnumTrait;

enum HariEnum: string
{
    use EnumTrait;

    case SENIN = 'SENIN';
    case SELASA = 'SELASA';
    case RABU = 'RABU';
    case KAMIS = 'KAMIS';
    case JUMAT = 'JUMAT';
    case SABTU = 'SABTU';
    case MINGGU = 'MINGGU';
}
