#include "Grid2.h"
#include "DrawDebugHelpers.h"

UGrid2::UGrid2()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UGrid2::BeginPlay()
{
    Super::BeginPlay();
}

FVector UGrid2::GetGridOriginOffset() const
{
    const float TotalX = GridSize.X * CellSize.X;
    const float TotalY = GridSize.Y * CellSize.Y;
    const FVector TotalSize(TotalX, TotalY, 0.f);

    switch (GridOrigin)
    {
    case EGridOrigin2::BottomLeft:
        return FVector(0.f, 0.f, 0.f);
    case EGridOrigin2::BottomRight:
        return FVector(-TotalSize.X, 0.f, 0.f);
    case EGridOrigin2::TopLeft:
        return FVector(0.f, -TotalSize.Y, 0.f);
    case EGridOrigin2::TopRight:
        return FVector(-TotalSize.X, -TotalSize.Y, 0.f);
    case EGridOrigin2::Center:
        return -TotalSize * 0.5f;
    default:
        return FVector::ZeroVector;
    }
}

FVector UGrid2::GetGridCenterWorld() const
{
    if (!GetOwner()) return FVector::ZeroVector;

    const FVector ActorLocation = GetOwner()->GetActorLocation();
    const FQuat ActorRotation = GetOwner()->GetActorQuat();

    FVector LocalCenter;
    if (GridType == EGridType::Isometric)
    {
        // 等距网格中心点：取中间格子
        const FIntPoint MidCell(GridSize.X / 2, GridSize.Y / 2);
        LocalCenter = CellToWorld(MidCell) - ActorLocation;
    }
    else
    {
        const FVector TotalDimensions(GridSize.X * CellSize.X, GridSize.Y * CellSize.Y, 0.f);
        LocalCenter = GetGridOriginOffset() + (TotalDimensions * 0.5f);
    }
    return ActorLocation + ActorRotation.RotateVector(LocalCenter);
}

FVector UGrid2::CellToWorld(const FIntPoint& Cell) const
{
    if (!GetOwner()) return FVector::ZeroVector;
    const FVector ActorLocation = GetOwner()->GetActorLocation();
    const FQuat ActorRotation = GetOwner()->GetActorQuat();

    FVector GridLocal(0,0,0);
    if (GridType == EGridType::Isometric)
    {
        const float HalfX = CellSize.X * 0.5f;
        const float HalfY = CellSize.Y * 0.5f;
        GridLocal.X = (Cell.X - Cell.Y) * HalfX;
        GridLocal.Y = (Cell.X + Cell.Y) * HalfY;
    }
    else
    {
        GridLocal.X = (Cell.X + 0.5f) * CellSize.X;
        GridLocal.Y = (Cell.Y + 0.5f) * CellSize.Y;
    }
    GridLocal += GetGridOriginOffset();
    return ActorLocation + ActorRotation.RotateVector(GridLocal);
}

FIntPoint UGrid2::WorldToCell(const FVector& WorldPos) const
{
    if (!GetOwner()) return FIntPoint::ZeroValue;
    const FVector ActorLocation = GetOwner()->GetActorLocation();
    const FQuat ActorRotation = GetOwner()->GetActorQuat();

    FVector LocalPos = ActorRotation.Inverse().RotateVector(WorldPos - ActorLocation);
    LocalPos -= GetGridOriginOffset();

    if (GridType == EGridType::Isometric)
    {
        const float HalfX = CellSize.X * 0.5f;
        const float HalfY = CellSize.Y * 0.5f;

        // 等距逆推公式
        const float u = LocalPos.X / HalfX;
        const float v = LocalPos.Y / HalfY;

        int32 X = FMath::FloorToInt((u + v) * 0.5f);
        int32 Y = FMath::FloorToInt((v - u) * 0.5f);
        return FIntPoint(X, Y);
    }
    else
    {
        return FIntPoint(
            FMath::FloorToInt(LocalPos.X / CellSize.X),
            FMath::FloorToInt(LocalPos.Y / CellSize.Y)
        );
    }
}

FIntPoint UGrid2::Mirror(FIntPoint Cell) const
{
    return FIntPoint(GridSize.X - 1 - Cell.X, Cell.Y);
}

bool UGrid2::IsCellValid(const FIntPoint& Cell) const
{
    return Cell.X >= 0 && Cell.Y >= 0 && Cell.X < GridSize.X && Cell.Y < GridSize.Y;
}

bool UGrid2::IsWorldPosInsideGrid(const FVector& WorldPos) const
{
    const FIntPoint Cell = WorldToCell(WorldPos);
    return IsCellValid(Cell);
}

#if !UE_BUILD_SHIPPING
void UGrid2::DrawDebugGrid() const
{
    if (!GetWorld() || !GetOwner()) return;
    const FVector ActorLoc = GetOwner()->GetActorLocation() + FVector(0,0,DebugLineZOffset);
    const FQuat ActorRot = GetOwner()->GetActorQuat();

    if (GridType == EGridType::Isometric)
    {
        // 内部网格斜线
        for (int32 Y = 0; Y < GridSize.Y; Y++)
        {
            for (int32 X = 0; X < GridSize.X; X++)
            {
                FIntPoint C0(X, Y);
                FIntPoint C1(X+1, Y);
                FIntPoint C2(X, Y+1);

                FVector P0 = CellToWorld(C0);
                FVector P1 = CellToWorld(C1);
                FVector P2 = CellToWorld(C2);

                DrawDebugLine(GetWorld(), P0, P1, DebugLineColor, false, 0, bIsDepthPriority, DebugLineThickness);
                DrawDebugLine(GetWorld(), P0, P2, DebugLineColor, false, 0, bIsDepthPriority, DebugLineThickness);
            }
        }

        // 四个外角
        FIntPoint CornerTR(GridSize.X, 0);
        FIntPoint CornerBL(0, GridSize.Y);
        FIntPoint CornerBR(GridSize.X, GridSize.Y);

        FVector P_TR = CellToWorld(CornerTR);
        FVector P_BL = CellToWorld(CornerBL);
        FVector P_BR = CellToWorld(CornerBR);

        // 补齐右下角两条收边斜边
        DrawDebugLine(GetWorld(), P_TR, P_BR, DebugLineColor, false, 0, bIsDepthPriority, DebugLineThickness);
        DrawDebugLine(GetWorld(), P_BL, P_BR, DebugLineColor, false, 0, bIsDepthPriority, DebugLineThickness);
    }
    else
    {
        // 直角绘制（优化版，无重复画线）
        const FVector OriginOffset = GetGridOriginOffset();
        const float TotalW = GridSize.X * CellSize.X;
        const float TotalH = GridSize.Y * CellSize.Y;
        for(int32 X = 0; X <= GridSize.X; X++)
        {
            float LX = X * CellSize.X + OriginOffset.X;
            FVector A(OriginOffset.X, OriginOffset.Y,0);
            FVector B(OriginOffset.X, OriginOffset.Y+TotalH,0);
            A.X = LX; B.X = LX;
            DrawDebugLine(GetWorld(), ActorLoc + ActorRot.RotateVector(A), ActorLoc + ActorRot.RotateVector(B),
                DebugLineColor, false, 0,bIsDepthPriority,DebugLineThickness);
        }
        for(int32 Y = 0; Y <= GridSize.Y; Y++)
        {
            float LY = Y * CellSize.Y + OriginOffset.Y;
            FVector A(OriginOffset.X, OriginOffset.Y,0);
            FVector B(OriginOffset.X+TotalW, OriginOffset.Y,0);
            A.Y = LY; B.Y = LY;
            DrawDebugLine(GetWorld(), ActorLoc + ActorRot.RotateVector(A), ActorLoc + ActorRot.RotateVector(B),
                DebugLineColor, false, 0,bIsDepthPriority,DebugLineThickness);
        }
    }
}

void UGrid2::DrawDebugCellText() const
{
    if (!GetWorld()) return;
    for (int x = 0; x < GridSize.X; ++x)
    {
        for (int y = 0; y < GridSize.Y; ++y)
        {
            const FIntPoint Cell(x, y);
            const FVector WorldPos = CellToWorld(Cell);
            DrawDebugString(
                GetWorld(),
                WorldPos + FVector(0.f, 0.f, DebugLineZOffset),
                FString::Printf(TEXT("(%d,%d)"), Cell.X, Cell.Y),
                nullptr,
                DebugTextColor,
                0.f,
                false,
                DebugTextScale
            );
        }
    }
}
#endif

void UGrid2::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
#if WITH_EDITOR
    if (!GetWorld() || !GetOwner()) return;
    if (bDrawDebugGrid) DrawDebugGrid();
    if (bDrawDebugCellText) DrawDebugCellText();
#endif
}
