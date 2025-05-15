<?php

namespace App\Filament\App\Resources;

use App\Enums\GenderEnum;
use App\Filament\App\Resources\GuruResource\Pages;
use App\Filament\App\Resources\GuruResource\RelationManagers\FingerprintsRelationManager;
use App\Models\Guru;
use BezhanSalleh\FilamentShield\Contracts\HasShieldPermissions;
use Filament\Forms;
use Filament\Forms\Form;
use Filament\Resources\Resource;
use Filament\Tables;
use Filament\Tables\Table;

class GuruResource extends Resource implements HasShieldPermissions
{
    protected static ?string $model = Guru::class;
    protected static ?string $slug = 'guru';
    protected static ?string $navigationGroup = 'Data Master';
    protected static ?string $recordTitleAttribute = 'nama';
    protected static ?string $navigationIcon = 'heroicon-o-user-group';

    public static function form(Form $form): Form
    {
        return $form
            ->schema([
                Forms\Components\TextInput::make('id')
                    ->label('ID')
                    ->integer()
                    ->readonly()
                    ->columnSpanFull()
                    ->hiddenOn('create')
                    ->dehydrated(false),
                Forms\Components\TextInput::make('nip')
                    ->label('NIP/NUPTK')
                    ->validationAttribute('NIP/NUPTK')
                    ->hint("18 digit NIP/16 digit NUPTK")
                    ->mask('999999999999999999')
                    ->placeholder('123XXXXXXXXXXXXXXX')
                    ->minLength(16)
                    ->integer()
                    ->autocomplete('nip')
                    ->unique(table: 'guru', ignoreRecord: true)
                    ->required()
                    ->columnSpanFull(),
                Forms\Components\TextInput::make('nama')
                    ->label('Nama Lengkap')
                    ->validationAttribute('Nama Lengkap')
                    ->string()
                    ->required()
                    ->autocomplete('full-name')
                    ->columnSpanFull(),
                Forms\Components\TextInput::make('no_hp')
                    ->label('No HP')
                    ->validationAttribute('No HP')
                    ->mask('+999999999999999')
                    ->placeholder('+628xxxxxxxxxx')
                    ->string()
                    ->tel()
                    ->required()
                    ->autocomplete('phone')
                    ->columnSpanFull(),
                Forms\Components\Radio::make('jenis_kelamin')
                    ->label('Jenis Kelamin')
                    ->validationAttribute('Jenis Kelamin')
                    ->options(GenderEnum::class)
                    ->enum(GenderEnum::class)
                    ->required(),
                Forms\Components\Textarea::make('alamat')
                    ->label('Alamat')
                    ->validationAttribute('Alamat')
                    ->maxLength(65535)
                    ->nullable()
                    ->autocomplete('address')
                    ->required()
                    ->columnSpanFull(),
            ]);
    }

    public static function table(Table $table): Table
    {
        return $table
            ->columns([
                Tables\Columns\TextColumn::make('id')
                    ->sortable()
                    ->label('ID'),
                Tables\Columns\TextColumn::make('nip')
                    ->sortable()
                    ->searchable()
                    ->label('NIP/NUPTK'),
                Tables\Columns\TextColumn::make('nama')
                    ->sortable()
                    ->searchable()
                    ->label('Nama'),
                Tables\Columns\TextColumn::make('jenis_kelamin')
                    ->sortable()
                    ->badge()
                    ->label('Jenis Kelamin'),
                Tables\Columns\TextColumn::make('no_hp')
                    ->sortable()
                    ->label('No HP'),
                Tables\Columns\TextColumn::make('created_at')
                    ->dateTime()
                    ->sortable()
                    ->toggleable(isToggledHiddenByDefault: true),
                Tables\Columns\TextColumn::make('updated_at')
                    ->dateTime()
                    ->sortable()
                    ->toggleable(isToggledHiddenByDefault: true),
            ])
            ->filters([
                //
            ])
            ->actions([
                Tables\Actions\ViewAction::make(),
                Tables\Actions\EditAction::make(),
                Tables\Actions\DeleteAction::make(),
            ])
            ->bulkActions([
                Tables\Actions\BulkActionGroup::make([
                    Tables\Actions\DeleteBulkAction::make(),
                ]),
            ])
            ->emptyStateActions([
                Tables\Actions\CreateAction::make(),
            ]);
    }

    public static function getRelations(): array
    {
        return [
            FingerprintsRelationManager::class,
        ];
    }

    public static function getPages(): array
    {
        return [
            'index' => Pages\ListGurus::route('/'),
            'create' => Pages\CreateGuru::route('/create'),
            'edit' => Pages\EditGuru::route('/{record}/edit'),
        ];
    }

    public static function getPermissionPrefixes(): array
    {
        return [
            'view',
            'view_any',
            'create',
            'update',
            'delete',
            'delete_any',
        ];
    }
}
