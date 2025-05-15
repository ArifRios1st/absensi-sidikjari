<?php

namespace App\Enums;

use App\Traits\EnumTrait;
use Filament\Support\Contracts\HasLabel;

enum GenderEnum:string implements HasLabel
{
    use EnumTrait;

    case MALE = "L";
    case FEMALE = "P";

    public function getLabel(): ?string
    {
        return match ($this) {
            self::MALE => 'Laki-laki',
            self::FEMALE => 'Perempuan',
        };
    }
}
